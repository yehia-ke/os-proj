#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>
#include "./queue/queue.h"
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"
#include <unistd.h>
#include <glib.h>
#include "./process/process.h"
#include "./popup/popup.h"

// Global variables for labels
GtkWidget *processnumlabel;
GtkWidget *clocklabel;
GtkWidget *schedulerlabel;
GtkWidget *userInputMutex;
GtkWidget *userOutputMutex;
GtkWidget *fileMutex;
Queue* TBD;
int pid = 1;
char* cscheduler = NULL;
GtkTreeStore *blockStore;
GtkTreeStore *processStore;
GtkTreeStore *readyStore;
GtkTreeStore *resourceblockStore;
GtkTreeStore *runningStore;
GtkWidget *main_window;
ManualClock *manualClock = NULL;
AutomaticClock *automaticClock = NULL;
char* clocktype = NULL;
process* tempp = NULL;

void update_gui();

void show_error_message(const char *message)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), // Use the global main window
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "%s",
        message);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void printtogui(char* message){
    g_print(message);
}

PCB *load_program(const char *filepath, int pid)
{
    PCB *process = create_process(pid, (char *)filepath);
    if (!process)
    {
        show_error_message("Process creation failed.");
        exit(1);
    }
    return process;
}

void checkArrivalTime(){
    Queue* tmp = queue_create();
    while(!queue_is_empty(TBD)){
        process* p = queue_dequeue(TBD);
        if((p->arrival_time == AutomaticClock_getCycle(automaticClock)) || (p->arrival_time == ManualClock_getCycle(manualClock))){
            PCB* hello = load_program(p->path,pid);
            pid++;
            add_process_to_scheduler(hello);
            g_print(hello->state);
        }
        else{
            queue_enqueue(tmp, p);
        }
    }
    while(!queue_is_empty(tmp)){
        process* p = queue_dequeue(tmp);
        queue_enqueue(TBD, p);
    }
    queue_destroy(tmp);
}

void* non_blocking_loop(void* arg) {
    while (automaticClock->is_running) {
        checkArrivalTime(); // Infinite loop
        run_scheduler();
        AutomaticClock_update(automaticClock);
        update_gui();
        //sleep(1); // Sleep for 1 second to avoid excessive CPU usage
        g_print("Automatic Clock updated.\n");
        usleep(1000 * 1000);
    }
    return NULL;
}

static char *get_current_timestamp()
{
    static char timestamp[64];
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", timeinfo);
    return timestamp;
}

static gboolean console_output_callback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
    char buf[256];
    gsize bytes_read;
    GError *error = NULL;

    g_io_channel_read_chars(source, buf, sizeof(buf) - 1, &bytes_read, &error);
    if (error)
    {
        g_error_free(error);
        return FALSE;
    }

    buf[bytes_read] = '\0';
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Prepend timestamp to the log message
    gtk_text_buffer_insert(buffer, &end, get_current_timestamp(), -1);
    gtk_text_buffer_insert(buffer, &end, buf, -1);

    return TRUE;
}

static void redirect_console_to_text_view(GtkWidget *log_text_view)
{
    static int pipe_fd[2];
    static GIOChannel *io_channel;

    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    io_channel = g_io_channel_unix_new(pipe_fd[0]);
    g_io_channel_set_flags(io_channel, G_IO_FLAG_NONBLOCK, NULL);

    g_io_add_watch(io_channel, G_IO_IN, console_output_callback, log_text_view);

    dup2(pipe_fd[1], STDOUT_FILENO);
    dup2(pipe_fd[1], STDERR_FILENO);
}


static void update_blockStore()
{
    gtk_tree_store_clear(blockStore);
    GtkTreeIter iter;
    gtk_tree_store_append(blockStore, &iter, NULL);
    gtk_tree_store_set(blockStore, &iter, 0, "Block PID 1", 1, "Block Inst 1", 2, "Block Time 1", -1);

    gtk_tree_store_append(blockStore, &iter, NULL);
    gtk_tree_store_set(blockStore, &iter, 0, "Block PID 2", 1, "Block Inst 2", 2, "Block Time 2", -1);
}

static void update_processStore()
{
    // Clear the existing data from the table
    gtk_tree_store_clear(processStore);

    // Get the process queue
    Queue *queue = get_process_queue();
    if (!queue || queue_is_empty(queue))
    {
        // If the queue is empty, there's nothing to display
        return;
    }

    // Create a temporary queue to hold processes while iterating
    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        // Dequeue a process from the original queue
        PCB *process = (PCB *)queue_dequeue(queue);

        // Add the process to the table
        GtkTreeIter iter;
        gtk_tree_store_append(processStore, &iter, NULL);
        gtk_tree_store_set(processStore, &iter,
                           0, process->pid,
                           1, process->state,
                           2, process->priority,
                           3, process->mem_lower,
                           4, process->mem_upper,
                           -1);

        // Enqueue the process into the temporary queue
        queue_enqueue(temp_queue, process);
    }

    // Restore all processes back to the original queue from the temporary queue
    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    // Destroy the temporary queue
    queue_destroy(temp_queue);
}

static void update_readyStore()
{
    gtk_tree_store_clear(readyStore);
    GtkTreeIter iter;
    gtk_tree_store_append(readyStore, &iter, NULL);
    gtk_tree_store_set(readyStore, &iter, 0, "Ready PID 1", 1, "Ready Inst 1", 2, "Ready Time 1", -1);

    gtk_tree_store_append(readyStore, &iter, NULL);
    gtk_tree_store_set(readyStore, &iter, 0, "Ready PID 2", 1, "Ready Inst 2", 2, "Ready Time 2", -1);
}

static void update_resourceblockStore()
{
    gtk_tree_store_clear(resourceblockStore);
    GtkTreeIter iter;
    gtk_tree_store_append(resourceblockStore, &iter, NULL);
    gtk_tree_store_set(resourceblockStore, &iter, 0, "Resource PID 1", 1, "Priority 1", -1);

    gtk_tree_store_append(resourceblockStore, &iter, NULL);
    gtk_tree_store_set(resourceblockStore, &iter, 0, "Resource PID 2", 1, "Priority 2", -1);
}

static void update_runningStore()
{
    gtk_tree_store_clear(runningStore);
    GtkTreeIter iter;
    gtk_tree_store_append(runningStore, &iter, NULL);
    gtk_tree_store_set(runningStore, &iter, 0, "Running PID 1", 1, "Running Inst 1", 2, "Running Time 1", -1);

    gtk_tree_store_append(runningStore, &iter, NULL);
    gtk_tree_store_set(runningStore, &iter, 0, "Running PID 2", 1, "Running Inst 2", 2, "Running Time 2", -1);
}

// Functions to set labels
void set_processnumlabel(const char *text)
{
    gtk_label_set_text(GTK_LABEL(processnumlabel), text);
}

void set_clocklabel(const char *text)
{
    gtk_label_set_text(GTK_LABEL(clocklabel), text);
}

void set_schedulerlabel(const char *text)
{
    gtk_label_set_text(GTK_LABEL(schedulerlabel), text);
}

void set_userInputMutex(const char *text)
{
    gtk_label_set_text(GTK_LABEL(userInputMutex), text);
}

void set_userOutputMutex(const char *text)
{
    gtk_label_set_text(GTK_LABEL(userOutputMutex), text);
}

void set_fileMutex(const char *text)
{
    gtk_label_set_text(GTK_LABEL(fileMutex), text);
}

void on_addprocessbutton_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Add Process Confirm button clicked.\n");
    if(tempp == NULL){
        show_error_message("No process selected.");
    }
    else if(tempp->path == NULL ){
        show_error_message("No path selected.");
    }
    else{
        g_print(tempp->path);
        g_print("%d",tempp->arrival_time);
        queue_enqueue(TBD, tempp);
        show_error_message("Process added successfully.");
    } 
    
}

void on_arrivaltimeinput_activate(GtkWidget *widget, gpointer data)
{
    g_print("Arrival time input activated: %s\n", gtk_entry_get_text(GTK_ENTRY(widget)));

    const char *input_text = gtk_entry_get_text(GTK_ENTRY(widget));
    int temptime = atoi(input_text);

    // Initialize tempp if it is NULL
    if (tempp == NULL)
    {
        tempp = malloc(sizeof(process));
        if (tempp == NULL)
        {
            show_error_message("Memory allocation failed for the process structure.");
            return;
        }
        tempp->path = NULL;
        tempp->arrival_time = 0; // Initialize fields to avoid garbage values
    }

    tempp->arrival_time = temptime;
    g_print("Arrival time set: %d\n", tempp->arrival_time);
}

void on_programfilepath_file_set(GtkWidget *widget, gpointer data)
{
    g_print("File selected: %s\n", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
    
    char *selected_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    
    // Initialize tempp if it is NULL
    if (tempp == NULL)
    {
        tempp = malloc(sizeof(process));
        if (tempp == NULL)
        {
            show_error_message("Memory allocation failed for the process structure.");
            return;
        }
        tempp->path = NULL;
        tempp->arrival_time = 0; // Initialize fields to avoid garbage values
    }

    // Allocate memory for the path and copy the selected file path
    tempp->path = malloc(strlen(selected_path) + 1);
    if (tempp->path == NULL)
    {
        show_error_message("Memory allocation failed for the process path.");
        free(tempp); // Free tempp to avoid memory leaks
        tempp = NULL;
        return;
    }
    strcpy(tempp->path, selected_path);
}

void on_fcfs_activate(GtkWidget *widget, gpointer data)
{
    if(clocktype == NULL){
        g_print("First Come First Serve selected.\n");
    set_scheduler("fcfs");
    cscheduler = "fcfs";
    set_schedulerlabel("Active Scheduler: FCFS");
    show_error_message("First Come First Serve selected.");
    }
    else{
        show_error_message("Scheduler already set.");
    }
}

void on_rr_activate(GtkWidget *widget, gpointer data)
{
    if(clocktype == NULL){
        g_print("Round Robin Selected.\n");
    set_scheduler("rr");
    cscheduler = "rr";
    set_schedulerlabel("Active Scheduler: RR");
    show_error_message("Round Robin Selected.");
    }
    else{
        show_error_message("Scheduler already set.");
    }
}

void on_mlfq_activate(GtkWidget *widget, gpointer data)
{
    if(clocktype == NULL){
        g_print("Multi Level Feedback Queue Selected.\n");
    set_scheduler("mlfq");
    cscheduler = "mlfq";
    set_schedulerlabel("Active Scheduler: MLFQ");
    show_error_message("Multi Level Feedback Queue selected.");
    }
    else{
        show_error_message("Scheduler already set.");
    }
}

void on_start_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Start Simulation clicked.\n");
    update_processStore();
}

void on_stop_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Stop Simulation clicked.\n");
}
void on_reset_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Reset Simulation clicked.\n");
}

void on_autoswitcher_clicked(GtkWidget *widget, gpointer data)
{
    if(cscheduler == NULL){
        show_error_message("No scheduler set.");
        return;
    }
    else{
        if(automaticClock != NULL){
            if(clocktype == NULL){
                clocktype = "a";
                AutomaticClock_start(automaticClock);
                g_print("Automatic Clock started.\n");
                pthread_t thread;
                pthread_create(&thread, NULL, non_blocking_loop, NULL);
                
            }
            else if(clocktype == "m"){
                clocktype = "a";
                int cc = ManualClock_getCycle(manualClock);
                automaticClock->current_cycle = cc;
                AutomaticClock_start(automaticClock);
                g_print("Automatic Clock started.\n");
                pthread_t thread;
                pthread_create(&thread, NULL, non_blocking_loop, NULL);
            }
            else{
                show_error_message("Automatic Clock is already running.");
            }
        }
    }
}

void on_manualstep_clicked(GtkWidget *widget, gpointer data)
{
    if(cscheduler == NULL){
        show_error_message("No scheduler set.");
        return;
    }
    else{
        if (manualClock != NULL && ((clocktype == NULL) || (clocktype == "m")))
    {
        clocktype = "m";
        g_print("Manual Step clicked.\n");
        checkArrivalTime();
        run_scheduler();
        ManualClock_tick(manualClock);
        update_gui();
        int clocktemp = ManualClock_getCycle(manualClock);
        char clockcyclemichael[20];
        sprintf(clockcyclemichael, "%d", clocktemp);
        char message[100];
        snprintf(message, sizeof(message), "Current Clock Cycle: %s", clockcyclemichael);
        set_clocklabel(message);
    }
    else if(clocktype == "a")
    {
        checkArrivalTime();
        run_scheduler();
        automaticClock->is_running = FALSE;
        int cc = AutomaticClock_getCycle(automaticClock);
        manualClock->current_cycle = cc;
        clocktype = "m";
        g_print("Manual Step clicked.\n");
        ManualClock_tick(manualClock);
        update_gui();
        int clocktemp = ManualClock_getCycle(manualClock);
        char clockcyclemichael[20];
        sprintf(clockcyclemichael, "%d", clocktemp);
        char message[100];
        snprintf(message, sizeof(message), "Current Clock Cycle: %s", clockcyclemichael);
        set_clocklabel(message);
        
    }
    }
}

void update_gui(){

    update_blockStore();
    update_processStore();
    update_readyStore();
    update_resourceblockStore();
    update_runningStore();

}

int main(int argc, char *argv[])
{
    TBD = queue_create();
    GtkBuilder *builder;
    GtkWidget *log_text_view;
    manualClock = ManualClock_create();
    automaticClock = AutomaticClock_create(0.5); // Default interval of 0.5 seconds
    manualClock->is_paused = FALSE;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/pt1.glade");

    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    log_text_view = GTK_WIDGET(gtk_builder_get_object(builder, "log_text_view"));

    // Initialize global labels
    processnumlabel = GTK_WIDGET(gtk_builder_get_object(builder, "processnumlabel"));
    clocklabel = GTK_WIDGET(gtk_builder_get_object(builder, "clocklabel"));
    schedulerlabel = GTK_WIDGET(gtk_builder_get_object(builder, "schedulerlabel"));
    userInputMutex = GTK_WIDGET(gtk_builder_get_object(builder, "userInputMutex"));
    userOutputMutex = GTK_WIDGET(gtk_builder_get_object(builder, "userOutputMutex"));
    fileMutex = GTK_WIDGET(gtk_builder_get_object(builder, "fileMutex"));

    redirect_console_to_text_view(log_text_view);

    blockStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "blockStore"));
    processStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "processStore"));
    readyStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "readyStore"));
    resourceblockStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "resourceblockStore"));
    runningStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "runningStore"));

    update_blockStore();
    update_processStore();
    update_readyStore();
    update_resourceblockStore();
    update_runningStore();

    gtk_builder_connect_signals(builder, NULL);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(main_window);

    gtk_main();

    return 0;
}

