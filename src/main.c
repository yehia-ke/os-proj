#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "./queue/queue.h"
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"
#include <unistd.h>

// Global variables
GtkTreeStore *blockStore;
GtkTreeStore *processStore;
GtkTreeStore *readyStore;
GtkTreeStore *resourceblockStore;
GtkTreeStore *runningStore;
GtkWidget *main_window; // Global main window variable

// Callback function for handling console output
static gboolean console_output_callback(GIOChannel *source, GIOCondition condition, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
    char buf[256];
    gsize bytes_read;
    GError *error = NULL;

    g_io_channel_read_chars(source, buf, sizeof(buf) - 1, &bytes_read, &error);
    if (error) {
        g_error_free(error);
        return FALSE;
    }

    buf[bytes_read] = '\0';
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, buf, -1);

    return TRUE;
}

// Redirect console output to GtkTextView
static void redirect_console_to_text_view(GtkWidget *log_text_view) {
    static int pipe_fd[2];
    static GIOChannel *io_channel;

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    io_channel = g_io_channel_unix_new(pipe_fd[0]);
    g_io_channel_set_flags(io_channel, G_IO_FLAG_NONBLOCK, NULL);

    g_io_add_watch(io_channel, G_IO_IN | G_IO_PRI, console_output_callback, log_text_view);

    dup2(pipe_fd[1], STDOUT_FILENO);
    dup2(pipe_fd[1], STDERR_FILENO);
}

void show_error_message(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), // Use the global main window
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "%s",
        message
    );

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Dummy data filling functions
static void update_blockStore() {
    gtk_tree_store_clear(blockStore);
    GtkTreeIter iter;
    gtk_tree_store_append(blockStore, &iter, NULL);
    gtk_tree_store_set(blockStore, &iter, 0, "Block PID 1", 1, "Block Inst 1", 2, "Block Time 1", -1);

    gtk_tree_store_append(blockStore, &iter, NULL);
    gtk_tree_store_set(blockStore, &iter, 0, "Block PID 2", 1, "Block Inst 2", 2, "Block Time 2", -1);
}

static void update_processStore() {
    GtkTreeIter iter;
    gtk_tree_store_append(processStore, &iter, NULL);
    gtk_tree_store_set(processStore, &iter, 0, "Process 1", 1, "State 1", 2, "Priority 1", 3, "Memory 1", 4, "Program 1", -1);

    gtk_tree_store_append(processStore, &iter, NULL);
    gtk_tree_store_set(processStore, &iter, 0, "Process 2", 1, "State 2", 2, "Priority 2", 3, "Memory 2", 4, "Program 2", -1);
}

static void update_readyStore() {
    gtk_tree_store_clear(readyStore);
    GtkTreeIter iter;
    gtk_tree_store_append(readyStore, &iter, NULL);
    gtk_tree_store_set(readyStore, &iter, 0, "Ready PID 1", 1, "Ready Inst 1", 2, "Ready Time 1", -1);

    gtk_tree_store_append(readyStore, &iter, NULL);
    gtk_tree_store_set(readyStore, &iter, 0, "Ready PID 2", 1, "Ready Inst 2", 2, "Ready Time 2", -1);
}

static void update_resourceblockStore() {
    gtk_tree_store_clear(resourceblockStore);
    GtkTreeIter iter;
    gtk_tree_store_append(resourceblockStore, &iter, NULL);
    gtk_tree_store_set(resourceblockStore, &iter, 0, "Resource PID 1", 1, "Priority 1", -1);

    gtk_tree_store_append(resourceblockStore, &iter, NULL);
    gtk_tree_store_set(resourceblockStore, &iter, 0, "Resource PID 2", 1, "Priority 2", -1);
}

static void update_runningStore() {
    gtk_tree_store_clear(runningStore);
    GtkTreeIter iter;
    gtk_tree_store_append(runningStore, &iter, NULL);
    gtk_tree_store_set(runningStore, &iter, 0, "Running PID 1", 1, "Running Inst 1", 2, "Running Time 1", -1);

    gtk_tree_store_append(runningStore, &iter, NULL);
    gtk_tree_store_set(runningStore, &iter, 0, "Running PID 2", 1, "Running Inst 2", 2, "Running Time 2", -1);
}

// Signal handlers
void on_addprocessbutton_clicked(GtkWidget *widget, gpointer data) {
    g_print("Add Process Confirm button clicked.\n");
    show_error_message("Process NOT added successfully.");
}

void on_arrivaltimeinput_activate(GtkWidget *widget, gpointer data) {
    g_print("Arrival time input activated: %s\n", gtk_entry_get_text(GTK_ENTRY(widget)));
}

void on_programfilepath_file_set(GtkWidget *widget, gpointer data) {
    g_print("File selected: %s\n", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
}

void on_fcfs_activate(GtkWidget *widget, gpointer data) {
    g_print("First Come First Serve selected.\n");
}

void on_rr_activate(GtkWidget *widget, gpointer data) {
    g_print("Round Robin selected.\n");
}

void on_mlfq_activate(GtkWidget *widget, gpointer data) {
    g_print("Multilevel Feedback Queue selected.\n");
}

void on_start_clicked(GtkWidget *widget, gpointer data) {
    g_print("Start Simulation clicked.\n");
    update_processStore();
}

void on_stop_clicked(GtkWidget *widget, gpointer data) {
    g_print("Stop Simulation clicked.\n");
}

void on_reset_clicked(GtkWidget *widget, gpointer data) {
    g_print("Reset Simulation clicked.\n");
}

void on_autoswitcher_clicked(GtkWidget *widget, gpointer data) {
    g_print("Auto Exec clicked.\n");
}

void on_manualstep_clicked(GtkWidget *widget, gpointer data) {
    g_print("Manual step clicked.\n");
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *log_text_view;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("./glade/pt1.glade");

    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "window")); // Set global main window
    log_text_view = GTK_WIDGET(gtk_builder_get_object(builder, "log_text_view"));

    redirect_console_to_text_view(log_text_view);

    // Initialize global TreeStores
    blockStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "blockStore"));
    processStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "processStore"));
    readyStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "readyStore"));
    resourceblockStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "resourceblockStore"));
    runningStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "runningStore"));

    // Fill TreeStores with dummy data
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