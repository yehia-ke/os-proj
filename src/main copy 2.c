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

// Global GtkTextView pointer for the log view
GtkTextView *global_log_view;

// Global GtkTreeStore pointers for the stores
GtkTreeStore *process_store = NULL;
GtkTreeStore *ready_store = NULL;
GtkTreeStore *block_store = NULL;
GtkTreeStore *running_store = NULL;
GtkTreeStore *resource_block_store = NULL;

// Function prototypes
void on_addprocessbutton_clicked(GtkButton *button, gpointer user_data);
void on_arrivaltimeinput_activate(GtkEntry *entry, gpointer user_data);
void on_programfilepath_file_set(GtkFileChooserButton *file_chooser, gpointer user_data);
void on_start_clicked(GtkButton *button, gpointer user_data);
void on_stop_clicked(GtkButton *button, gpointer user_data);
void on_reset_clicked(GtkButton *button, gpointer user_data);
void on_fcfs_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_rr_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_mlfq_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_autoswitcher_clicked(GtkButton *button, gpointer user_data);
void on_manualstep_clicked(GtkButton *button, gpointer user_data);
void update_process_store();
void update_ready_store();
void update_block_store();
void update_running_store();
void update_resource_block_store();
void append_to_log(const gchar *message);
const gchar *get_timestamp();

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("./glade/pt1.glade");

    // Connect signals
    gtk_builder_connect_signals(builder, builder);

    // Get the main window
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Initialize the global log view
    global_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "log_text_view"));

    // Initialize the global tree stores
    process_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "processStore"));
    ready_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "readyStore"));
    block_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "blockStore"));
    running_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "runningStore"));
    resource_block_store = GTK_TREE_STORE(gtk_builder_get_object(builder, "resourceblockStore"));

    // Add debug logs to verify initialization
    if (process_store == NULL)
        g_warning("Process store is not initialized.");
    if (ready_store == NULL)
        g_warning("Ready store is not initialized.");
    if (block_store == NULL)
        g_warning("Block store is not initialized.");
    if (running_store == NULL)
        g_warning("Running store is not initialized.");
    if (resource_block_store == NULL)
        g_warning("Resource block store is not initialized.");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

void on_addprocessbutton_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Add Process button clicked");
    // Implement logic to add a process
}

void on_arrivaltimeinput_activate(GtkEntry *entry, gpointer user_data) {
    const gchar *arrival_time = gtk_entry_get_text(entry);
    gchar *log_message = g_strdup_printf("Arrival Time Input activated: %s", arrival_time);
    append_to_log(log_message);
    g_free(log_message);
}

void on_programfilepath_file_set(GtkFileChooserButton *file_chooser, gpointer user_data) {
    gchar *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    gchar *log_message = g_strdup_printf("Program file path set: %s", file_path);
    append_to_log(log_message);
    g_free(log_message);
    g_free(file_path);
}

void on_start_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Start Simulation clicked");
    update_process_store();
    update_ready_store();
    update_block_store();
    update_running_store();
    update_resource_block_store();
    
}

void on_stop_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Stop Simulation clicked");
    // Stop simulation logic here
}

void on_reset_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Reset Simulation clicked");
    // Reset simulation logic here
}

void on_fcfs_activate(GtkMenuItem *menuitem, gpointer user_data) {
    append_to_log("FCFS scheduling selected");
    // Logic for FCFS scheduling
}

void on_rr_activate(GtkMenuItem *menuitem, gpointer user_data) {
    append_to_log("Round Robin scheduling selected");
    // Logic for Round Robin scheduling
}

void on_mlfq_activate(GtkMenuItem *menuitem, gpointer user_data) {
    append_to_log("Multilevel Feedback Queue scheduling selected");
    // Logic for Multilevel Feedback Queue scheduling
}

void on_autoswitcher_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Auto Exec clicked");
    // Logic for Auto Execution
}

void on_manualstep_clicked(GtkButton *button, gpointer user_data) {
    append_to_log("Manual Step clicked");
    // Logic for the next step
}

void update_process_store() {
    if (process_store == NULL) {
        g_warning("Process store is NULL. Cannot update.");
        return;
    }

    GtkTreeIter iter;
    gtk_tree_store_clear(process_store);

    // Example: Add a dummy process entry
    gtk_tree_store_append(process_store, &iter, NULL);
    gtk_tree_store_set(process_store, &iter,
        0, "P1",            // Process ID
        1, "Running",       // Process State
        2, "High",          // Current Priority
        3, "0x1000-0x2000", // Memory Boundaries
        4, "MainProgram",   // Program Counter
        -1);
}

void update_ready_store() {
    if (ready_store == NULL) {
        g_warning("Ready store is NULL. Cannot update.");
        return;
    }

    GtkTreeIter iter;
    gtk_tree_store_clear(ready_store);

    // Example: Add a dummy ready process entry
    gtk_tree_store_append(ready_store, &iter, NULL);
    gtk_tree_store_set(ready_store, &iter,
        0, "P2",            // Process ID
        1, "Instruction1",  // Instruction
        2, "5",             // Time
        -1);
}

void update_block_store() {
    if (block_store == NULL) {
        g_warning("Block store is NULL. Cannot update.");
        return;
    }

    GtkTreeIter iter;
    gtk_tree_store_clear(block_store);

    // Example: Add a dummy blocked process entry
    gtk_tree_store_append(block_store, &iter, NULL);
    gtk_tree_store_set(block_store, &iter,
        0, "P3",            // Process ID
        1, "Instruction2",  // Instruction
        2, "10",            // Time
        -1);
}

void update_running_store() {
    if (running_store == NULL) {
        g_warning("Running store is NULL. Cannot update.");
        return;
    }

    GtkTreeIter iter;
    gtk_tree_store_clear(running_store);

    // Example: Add a dummy running process entry
    gtk_tree_store_append(running_store, &iter, NULL);
    gtk_tree_store_set(running_store, &iter,
        0, "P4",            // Process ID
        1, "Instruction3",  // Instruction
        2, "3",             // Time
        -1);
}

void update_resource_block_store() {
    if (resource_block_store == NULL) {
        g_warning("Resource block store is NULL. Cannot update.");
        return;
    }

    GtkTreeIter iter;
    gtk_tree_store_clear(resource_block_store);

    // Example: Add a dummy resource blocking entry
    gtk_tree_store_append(resource_block_store, &iter, NULL);
    gtk_tree_store_set(resource_block_store, &iter,
        0, "P5",            // Process ID
        1, "Low",           // Priority
        -1);
}

void append_to_log(const gchar *message) {
    if (!global_log_view) {
        fprintf(stderr, "Log view is not initialized.\n");
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_log_view);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Format the message with a timestamp
    gchar *timestamped_message = g_strdup_printf("[%s] %s\n", get_timestamp(), message);
    gtk_text_buffer_insert(buffer, &end, timestamped_message, -1);
    g_free(timestamped_message);

    // Automatically scroll to the new line
    gtk_text_view_scroll_to_iter(global_log_view, &end, 0.0, FALSE, 0.0, 1.0);
}

const gchar *get_timestamp() {
    static gchar timestamp[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    return timestamp;
}