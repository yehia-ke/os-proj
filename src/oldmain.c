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

// Global variables for GUI components
GtkWidget *process_table;
GtkListStore *process_store;
GtkWidget *scheduler_dropdown;
GtkWidget *manual_clock_radio;
GtkWidget *auto_clock_radio;
GtkWidget *start_button;

// Function to load a program from a file into a PCB
PCB *load_program(const char *filename, int pid, int arrival_time)
{
    PCB *process = create_process(pid, (char *)filename, arrival_time);
    if (!process)
    {
        printf("Failed to load program: %s\n", filename);
        exit(1);
    }
    return process;
}

// Function to update the process table in the GUI
void update_process_table()
{
    // Clear the existing data from the table
    gtk_list_store_clear(process_store);

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
        gtk_list_store_append(process_store, &iter);
        gtk_list_store_set(process_store, &iter,
                           0, process->pid,
                           1, process->state,
                           2, process->priority,
                           3, process->mem_lower,
                           4, process->mem_upper,
                           5, process->pc,
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

// Function to create the process table with headers
GtkWidget *create_process_table()
{
    // Create a list store with columns: Process ID, State, Priority, Memory Lower Bound, Memory Upper Bound, Program Counter
    process_store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

    // Create a tree view to display the table
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(process_store));

    // Add column headers
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Column 1: Process ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Process ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 2: Process State
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Process State", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 3: Current Priority
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Current Priority", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 4: Memory Lower Bound
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Memory Lower Bound", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 5: Memory Upper Bound
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Memory Upper Bound", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 6: Program Counter
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Program Counter", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    return tree_view;
}

// Function to log events in the GUI console
void log_event(const char *message)
{
    printf("%s\n", message);  // Log message to console for now
}

// Callback for the Start button
void start_simulation(GtkWidget *widget, gpointer data)
{
    log_event("Simulation Started");
    update_process_table(); // Update process table when the simulation starts
    // TODO: Start the simulation loop and update the GUI periodically
}

// Callback for Scheduler dropdown
void scheduler_changed(GtkComboBox *widget, gpointer data)
{
    const char *scheduler = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    if (scheduler != NULL)
    {
        log_event("Scheduler changed to:");
        log_event(scheduler);

        // Fix: Remove the const qualifier by creating a mutable copy
        char mutable_scheduler[10];
        strncpy(mutable_scheduler, scheduler, sizeof(mutable_scheduler) - 1);
        mutable_scheduler[sizeof(mutable_scheduler) - 1] = '\0';

        set_scheduler(mutable_scheduler); // Set the selected scheduler
    }
}

// Callback for Clock type radio button
void clock_type_changed(GtkToggleButton *widget, gpointer data)
{
    gboolean active = gtk_toggle_button_get_active(widget);
    if (active)
    {
        log_event("Clock type set to Automatic");
        // TODO: Set clock type to automatic
    }
    else
    {
        log_event("Clock type set to Manual");
        // TODO: Set clock type to manual
    }
}

// Main function
int main(int argc, char *argv[])
{
    // Initialize GTK
    set_scheduler("fcfs");
    PCB* process = load_program("Program_1.txt",1,0);
    add_process_to_scheduler(process);
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OS Simulation GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a grid to layout GUI components
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Scheduler Dropdown
    GtkWidget *scheduler_label = gtk_label_new("Scheduler:");
    gtk_grid_attach(GTK_GRID(grid), scheduler_label, 0, 0, 1, 1);

    scheduler_dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "fcfs");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "mlfq");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "rr");
    gtk_grid_attach(GTK_GRID(grid), scheduler_dropdown, 1, 0, 1, 1);
    g_signal_connect(scheduler_dropdown, "changed", G_CALLBACK(scheduler_changed), NULL);

    // Clock Type Radio Buttons
    GtkWidget *clock_type_label = gtk_label_new("Clock Type:");
    gtk_grid_attach(GTK_GRID(grid), clock_type_label, 0, 1, 1, 1);

    manual_clock_radio = gtk_radio_button_new_with_label(NULL, "Manual");
    gtk_grid_attach(GTK_GRID(grid), manual_clock_radio, 1, 1, 1, 1);
    g_signal_connect(manual_clock_radio, "toggled", G_CALLBACK(clock_type_changed), NULL);

    auto_clock_radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(manual_clock_radio), "Automatic");
    gtk_grid_attach(GTK_GRID(grid), auto_clock_radio, 2, 1, 1, 1);

    // Start Button
    start_button = gtk_button_new_with_label("Start Simulation");
    gtk_grid_attach(GTK_GRID(grid), start_button, 0, 2, 3, 1);
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_simulation), NULL);

    // Process Table
    GtkWidget *process_label = gtk_label_new("Process Queue:");
    gtk_grid_attach(GTK_GRID(grid), process_label, 0, 3, 1, 1);

    process_table = create_process_table();
    gtk_grid_attach(GTK_GRID(grid), process_table, 0, 4, 4, 1);

    // Show all widgets
    gtk_widget_show_all(window);

    // Enter the GTK main loop
    gtk_main();

    return 0;
}