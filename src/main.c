#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>
#include "./queue/queue.h"
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "interpreter/mutex.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"
#include "./memory/memory.h"
#include <unistd.h>
// Ensure this line is present to include GLib definitions
#include <glib.h>
#include "./process/process.h"
#include "./popup/popup.h"
#include "./error/error.h"

// Global variables for labels
GtkWidget *processnumlabel;
GtkWidget *clocklabel;
GtkWidget *schedulerlabel;
GtkWidget *userInputMutex;
GtkWidget *userOutputMutex;
GtkWidget *fileMutex;
Queue *TBD;
int pid = 1;
char *cscheduler = NULL;
GtkTreeStore *blockStore;
GtkTreeStore *processStore;
GtkTreeStore *readyStore;
GtkTreeStore *resourceblockStore;
GtkTreeStore *runningStore;
GtkBox *memoryBoxes[60];
GtkStyleContext *context;
GtkWidget *main_window;
ManualClock *manualClock = NULL;
AutomaticClock *automaticClock = NULL;
char *clocktype = NULL;
process *tempp = NULL;

GtkWidget *loading_window;

Queue* current_process_queue = NULL;

gboolean perform_reset(gpointer data) {
    // Restart the app
    execl("./bin/program", "Fire Scheduler", NULL);

    // If execl fails
    perror("Failed to restart application");
    exit(1);
    return FALSE; // Stop the timeout
}

void reset_application() {
    // Hide the main window
    gtk_widget_hide(main_window);

    // Create loading window
    loading_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(loading_window), "Please wait...");
    gtk_window_set_default_size(GTK_WINDOW(loading_window), 300, 100);
    gtk_window_set_position(GTK_WINDOW(loading_window), GTK_WIN_POS_CENTER);

    GtkWidget *label = gtk_label_new("Resetting application...");
    gtk_container_add(GTK_CONTAINER(loading_window), label);
    gtk_widget_show_all(loading_window);

    // Wait 1 second, then perform reset
    g_timeout_add(1000, perform_reset, NULL); // 1000 ms = 1 second
}


void update_gui();

void printtogui(char *message)
{
    g_print(message);
}

PCB *load_program(const char *filepath, int pid)
{
    PCB *process = create_process(pid, (char *)filepath);
    return process;
}

void checkArrivalTime()
{
    Queue *tmp = queue_create();
    while (!queue_is_empty(TBD))
    {
        process *p = queue_dequeue(TBD);
        if ((p->arrival_time == AutomaticClock_getCycle(automaticClock)) || (p->arrival_time == ManualClock_getCycle(manualClock)))
        {
            PCB *hello = load_program(p->path, pid);
            char output[50];
            sprintf(output, "PID %d failed to be created", pid);
            if(!hello){
                show_error_message(output);
            }
            else{
            pid++;
            add_process_to_scheduler(hello);
            g_print(hello->state);
        }
        }
        else
        {
            queue_enqueue(tmp, p);
        }
    }
    while (!queue_is_empty(tmp))
    {
        process *p = queue_dequeue(tmp);
        queue_enqueue(TBD, p);
    }
    queue_destroy(tmp);
}

void *non_blocking_loop(void *arg)
{
    while (automaticClock->is_running)
    {
        checkArrivalTime(); // Infinite loop
        run_scheduler();
        AutomaticClock_update(automaticClock);
        g_idle_add((GSourceFunc)update_gui, NULL);
        // sleep(1); // Sleep for 1 second to avoid excessive CPU usage
        g_print("Automatic Clock updated.\n");
        usleep(1500 * 1000);
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
    GtkTextView *text_view = GTK_TEXT_VIEW(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
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

    gtk_text_buffer_insert(buffer, &end, buf, -1);

    // Scroll to the bottom
    gtk_text_view_scroll_to_iter(text_view, &end, 0.0, FALSE, 0.0, 1.0);

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

    Queue *queue = get_block_queue();
    if (!queue || queue_is_empty(queue))
    {
        return;
    }

    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        PCB *process = (PCB *)queue_dequeue(queue);
        if (process)
        {
            GtkTreeIter iter;
            gtk_tree_store_append(blockStore, &iter, NULL);
            char pid[20];
            sprintf(pid, "%d", process->pid);
            char tiq[20];
            sprintf(tiq, "%d", process->tiq);

            gtk_tree_store_set(blockStore, &iter, 0, pid, 1, get_instruction(process), 2, tiq, -1);
            queue_enqueue(temp_queue, process);
        }
        else
        {
            g_print("Process is NULL\n");
        }
    }

    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    queue_destroy(temp_queue);
}

static void update_memoryAndProcessStore()
{
    gtk_tree_store_clear(processStore);

    Queue *queue = current_process_queue;
    if (!queue || queue_is_empty(queue))
    {
        return;
    }

    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        PCB *process = (PCB *)queue_dequeue(queue);
        if (process)
        {
            GtkTreeIter iter;
            gtk_tree_store_append(processStore, &iter, NULL);
            char pid[20];
            sprintf(pid, "%d", process->pid);
            char priority[20];
            sprintf(priority, "%d", process->priority);
            char mem_lower[20];
            sprintf(mem_lower, "%d", process->mem_lower);
            char mem_upper[20];
            sprintf(mem_upper, "%d", process->mem_upper);
            char pc[20];
            sprintf(pc, "%d", process->pc);
            strcat(mem_lower, " - ");
            strcat(mem_lower, mem_upper);
            gtk_tree_store_set(processStore, &iter, 0, pid, 1, process->state, 2, priority, 3, mem_lower, 4, pc, -1);

            // Update the memory grid
            for (int i = process->mem_lower; i <= process->mem_upper; i++)
            {
                char *memory_word = get_memory_word(i);
                GtkBox *box = memoryBoxes[i];
                char *varName;

                // Determine varName based on memory_word
                if (i >= process->mem_lower && i <= process->mem_lower + 5) {
                    char *start = strchr(memory_word, '_') + 1;
                    char *end = strchr(start, ':');
                    if (start && end && end > start) {
                        size_t length = end - start;
                        varName = malloc(length + 1);
                        strncpy(varName, start, length);
                        varName[length] = '\0';
                    } else {
                        varName = NULL;
                    }
                } else if (i >= process->mem_lower + 6) {
                    char *start = memory_word;
                    char *end = strchr(memory_word, ':');
                    if (end && end > start) {
                        size_t length = end - start;
                        varName = malloc(length + 1);
                        strncpy(varName, start, length);
                        varName[length] = '\0';
                    } else {
                        varName = NULL;
                    }
                }

                // Allocate memory for the label text
                char *temp = malloc(50 * sizeof(char));
                sprintf(temp, "%d: Process %d %s", i, process->pid, varName);

                // Ensure the box contains a valid child widget
                if (GTK_IS_BOX(box)) {
                    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
                    if (children != NULL) {
                        GtkWidget *label = GTK_WIDGET(children->data);
                        if (GTK_IS_LABEL(label)) {
                            gtk_label_set_text(GTK_LABEL(label), temp);
                        }
                        g_list_free(children);
                    }
                } else{
                    g_print("Box is not a valid GTK Box\n");
                }

                // Update the style context
                if (GTK_IS_WIDGET(box)) {
                    GtkStyleContext *box_context = gtk_widget_get_style_context(GTK_WIDGET(box));
                    if (box_context != NULL) {
                            gtk_style_context_remove_class(box_context, "memory-box");
                            gtk_style_context_add_class(box_context, "memory-box-red");
                    } else {
                        g_print("Failed to retrieve style context for the box.\n");
                    }
                    char *tooltip_text = strchr(get_memory_word(i), ':') + 1;
                    if (tooltip_text != NULL) {
                        gtk_widget_set_tooltip_text(GTK_WIDGET(box), tooltip_text);
                    } else {
                        gtk_widget_set_tooltip_text(GTK_WIDGET(box), "No tooltip available");
                    }
                } else {
                    g_print("Box is not a valid GTK Widget.\n");
                }

                free(temp); // Free allocated memory
            }
            
            queue_enqueue(temp_queue, process);
        }
        else
        {
            g_print("Process is NULL\n");
        }
    }

    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    queue_destroy(temp_queue);
}

static void clear_memoryBoxes()
{
    int j = 0;
    while (j < 60)
    {
        GList *children = gtk_container_get_children(GTK_CONTAINER(memoryBoxes[j]));
        GtkWidget *label = GTK_WIDGET(children->data);
        //show_error_message(gtk_label_get_text(GTK_LABEL(label)));
        if (GTK_IS_LABEL(label)) {
            char *temp = malloc(50 * sizeof(char));
            sprintf(temp, "%d: Free", j);
            gtk_label_set_text(GTK_LABEL(label), temp);
        }

        GtkStyleContext *box_context = gtk_widget_get_style_context(GTK_WIDGET(memoryBoxes[j]));
        if (box_context != NULL) {
            if (gtk_style_context_has_class(box_context, "memory-box-red")) {
                gtk_style_context_remove_class(box_context, "memory-box-red");
            }
            gtk_style_context_add_class(box_context, "memory-box");
        } else {
            g_print("Failed to retrieve style context for the box.\n");
        }
        j++;
    }
}

static void update_readyStore()
{
    gtk_tree_store_clear(readyStore);

    Queue *queue = get_ready_queue();
    if (!queue || queue_is_empty(queue))
    {
        return;
    }

    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        PCB *process = (PCB *)queue_dequeue(queue);
        if (process)
        {
            GtkTreeIter iter;
            gtk_tree_store_append(readyStore, &iter, NULL);
            char pid[20];
            sprintf(pid, "%d", process->pid);
            char tiq[20];
            sprintf(tiq, "%d", process->tiq);

            gtk_tree_store_set(readyStore, &iter, 0, pid, 1, get_instruction(process), 2, tiq, -1);
            queue_enqueue(temp_queue, process);
        }
        else
        {
            g_print("Process is NULL\n");
        }
    }

    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    queue_destroy(temp_queue);
}

static void update_resourceblockStore()
{
    gtk_tree_store_clear(resourceblockStore);

    Queue *queue = get_block_queue();
    if (!queue || queue_is_empty(queue))
    {
        return;
    }

    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        PCB *process = (PCB *)queue_dequeue(queue);
        if (process)
        {
            GtkTreeIter iter;
            gtk_tree_store_append(resourceblockStore, &iter, NULL);
            char pid[20];
            sprintf(pid, "%d", process->pid);
            char priority[20];
            sprintf(priority, "%d", process->priority);

            gtk_tree_store_set(resourceblockStore, &iter, 0, pid, 1,priority, -1);
            queue_enqueue(temp_queue, process);
        }
        else
        {
            g_print("Process is NULL\n");
        }
    }

    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    queue_destroy(temp_queue);
}

static void update_runningStore()
{
    gtk_tree_store_clear(runningStore);

    Queue *queue = get_run_queue();
    if (!queue || queue_is_empty(queue))
    {
        return;
    }

    Queue *temp_queue = queue_create();

    while (!queue_is_empty(queue))
    {
        PCB *process = (PCB *)queue_dequeue(queue);
        if (process)
        {
            GtkTreeIter iter;
            gtk_tree_store_append(runningStore, &iter, NULL);
            char pid[20];
            sprintf(pid, "%d", process->pid);
            char tiq[20];
            sprintf(tiq, "%d", process->tiq);

            gtk_tree_store_set(runningStore, &iter, 0, pid, 1, get_instruction(process), 2, tiq, -1);
            queue_enqueue(temp_queue, process);
        }
        else
        {
            g_print("Process is NULL\n");
        }
    }

    while (!queue_is_empty(temp_queue))
    {
        PCB *process = (PCB *)queue_dequeue(temp_queue);
        queue_enqueue(queue, process);
    }

    queue_destroy(temp_queue);
}

// Functions to set labels
void set_processnumlabel()
{
    char processnum[50];
    int processnumber = queue_size(current_process_queue);
    sprintf(processnum, "%d", processnumber);
    char final[50] = "Number of Processes: ";
    strcat(final, processnum);
    gtk_label_set_text(GTK_LABEL(processnumlabel), final);
}

void set_clocklabel(const char *text)
{
    gtk_label_set_text(GTK_LABEL(clocklabel), text);
}

void set_schedulerlabel(const char *text)
{
    gtk_label_set_text(GTK_LABEL(schedulerlabel), text);
}

void set_userInputMutex()
{
    char text[50] = "UserInput: Process ";
    char *mutex = getMutex(0);
    if(strcmp(mutex,"")){
        strcat(text, mutex);
    }
    else{
        strcat(text, "NONE");
    }
    gtk_label_set_text(GTK_LABEL(userInputMutex), text);
}

void set_userOutputMutex()
{
    char text[50] = "UserOutput: Process ";
    char *mutex = getMutex(1);
    if(strcmp(mutex,"")){
        strcat(text, mutex);
    }
    else{
        strcat(text, "NONE");
    }
    
    gtk_label_set_text(GTK_LABEL(userOutputMutex), text);
}

void set_fileMutex()
{
    char text[50] = "File: Process ";
    char *mutex = getMutex(2);
    if(strcmp(mutex,"")){
        strcat(text, mutex);
    }
    else{
        strcat(text, "NONE");
    }
    gtk_label_set_text(GTK_LABEL(fileMutex), text);
}

void on_addprocessbutton_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Add Process Confirm button clicked.\n");
    if (tempp == NULL)
    {
        show_error_message("No process selected.");
    }
    else if (tempp->path == NULL)
    {
        show_error_message("No path selected.");
    }
    else if (tempp->arrival_time < AutomaticClock_getCycle(automaticClock) || tempp->arrival_time < ManualClock_getCycle(manualClock))
    {
        show_error_message("Arrival Time cannot be less than the current clock cycle.");
    }
    else
    {
        process *processpp = malloc(sizeof(process));
        processpp->arrival_time = tempp->arrival_time;
        processpp->path = tempp->path;
        g_print("Process File Path:%s, Arrival Time:%d\n", processpp->path, processpp->arrival_time);
        queue_enqueue(TBD, processpp);
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
    if (clocktype == NULL)
    {
        g_print("First Come First Serve selected.\n");
        set_scheduler("fcfs");
        cscheduler = "fcfs";
        set_schedulerlabel("Active Scheduler: FCFS");
        show_error_message("First Come First Serve selected.");
    }
    else
    {
        show_error_message("Scheduler already set.");
    }
}

void on_rr_activate(GtkWidget *widget, gpointer data)
{
    if (clocktype == NULL)
    {
        char* quanta = show_text_entry_popup("Please input time quantum");
        int quantum = atoi(quanta);
        set_rr_quantum(quantum);
        set_scheduler("rr");
        cscheduler = "rr";
        g_print("Round Robin Selected.\n");
        set_schedulerlabel("Active Scheduler: RR");
        show_error_message("Round Robin Selected.");
        g_print("Round Robin Selected.\n");
    }
    else
    {
        show_error_message("Scheduler already set.");
    }
}

void on_mlfq_activate(GtkWidget *widget, gpointer data)
{
    if (clocktype == NULL)
    {
        g_print("Multi Level Feedback Queue Selected.\n");
        set_scheduler("mlfq");
        cscheduler = "mlfq";
        set_schedulerlabel("Active Scheduler: MLFQ");
        show_error_message("Multi Level Feedback Queue selected.");
    }
    else
    {
        show_error_message("Scheduler already set.");
    }
}


void on_reset_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Reset Simulation clicked.\n");
    reset_application();
}

void on_autoswitcher_clicked(GtkWidget *widget, gpointer data)
{
    if (cscheduler == NULL)
    {
        show_error_message("No scheduler set.");
        return;
    }
    else
    {
        if (automaticClock != NULL)
        {
            if (clocktype == NULL)
            {
                clocktype = "a";
                AutomaticClock_start(automaticClock);
                g_print("Automatic Clock started.\n");
                pthread_t thread;
                pthread_create(&thread, NULL, non_blocking_loop, NULL);
            }
            else if (clocktype == "m")
            {
                clocktype = "a";
                int cc = ManualClock_getCycle(manualClock);
                automaticClock->current_cycle = cc;
                AutomaticClock_start(automaticClock);
                g_print("Automatic Clock started.\n");
                pthread_t thread;
                pthread_create(&thread, NULL, non_blocking_loop, NULL);
            }
            else
            {
                show_error_message("Automatic Clock is already running.");
            }
        }
    }
}

void on_manualstep_clicked(GtkWidget *widget, gpointer data)
{
    if (cscheduler == NULL)
    {
        show_error_message("No scheduler set.");
        return;
    }
    else
    {
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
        else if (clocktype == "a")
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

void update_gui()
{
    current_process_queue = get_process_queue();
    update_blockStore();
    clear_memoryBoxes();
    update_memoryAndProcessStore();
    update_readyStore();
    update_resourceblockStore();
    update_runningStore();
    set_processnumlabel();
    set_userInputMutex();
    set_userOutputMutex();
    set_fileMutex();
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

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "src/glade/style.css", NULL);

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

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

    for (int i = 0; i < 60; i++)
    {
        char box_name[20];
        sprintf(box_name, "box%d", i);
        memoryBoxes[i] = GTK_BOX(gtk_builder_get_object(builder, box_name));
    }

   

    gtk_builder_connect_signals(builder, NULL);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(main_window);

    gtk_main();

    return 0;
}
