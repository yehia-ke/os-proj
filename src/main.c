#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"

// Global variables for GUI components
GtkWidget *process_list;
GtkWidget *memory_viewer;
GtkWidget *log_console;
GtkWidget *scheduler_dropdown;
GtkWidget *clock_type_radio;
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

// Function to update the process list in the GUI
void update_process_list()
{
    // Example data for a queue format
    const char *process_queue =
        "Process Queue:\n"
        "--------------------\n"
        "PID   | State   | Priority | Memory Bounds | Program Counter\n"
        "1     | Running | High     | 0-15          | 10\n"
        "2     | Ready   | Medium   | 16-30         | 5\n"
        "3     | Blocked | Low      | 31-45         | 0\n";

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(process_list))),
                             process_queue, -1);
}

// Function to update the memory viewer in the GUI
void update_memory_viewer()
{
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(memory_viewer))),
                             "Memory Viewer Updated: (Example Data)\n[0] Process 1\n[1] Process 2\n[2] Empty", -1);
}

// Function to log events in the GUI console
void log_event(const char *message)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_console));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
}

// Callback for the Start button
void start_simulation(GtkWidget *widget, gpointer data)
{
    log_event("Simulation Started");
    update_process_list(); // Update process list when the simulation starts
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
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OS Simulation GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a grid to layout GUI components
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Memory Viewer
    GtkWidget *memory_label = gtk_label_new("Memory Viewer:");
    gtk_grid_attach(GTK_GRID(grid), memory_label, 0, 0, 1, 1);

    memory_viewer = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(memory_viewer), FALSE);
    gtk_grid_attach(GTK_GRID(grid), memory_viewer, 0, 1, 4, 1);

    // Log Console
    GtkWidget *log_label = gtk_label_new("Log Console:");
    gtk_grid_attach(GTK_GRID(grid), log_label, 0, 2, 1, 1);

    log_console = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(log_console), FALSE);
    gtk_grid_attach(GTK_GRID(grid), log_console, 0, 3, 4, 1);

    // Scheduler Dropdown
    GtkWidget *scheduler_label = gtk_label_new("Scheduler:");
    gtk_grid_attach(GTK_GRID(grid), scheduler_label, 0, 4, 1, 1);

    scheduler_dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "fcfs");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "mlfq");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(scheduler_dropdown), NULL, "rr");
    gtk_grid_attach(GTK_GRID(grid), scheduler_dropdown, 1, 4, 1, 1);
    g_signal_connect(scheduler_dropdown, "changed", G_CALLBACK(scheduler_changed), NULL);

    // Clock Type Radio Buttons
    GtkWidget *clock_type_label = gtk_label_new("Clock Type:");
    gtk_grid_attach(GTK_GRID(grid), clock_type_label, 2, 4, 1, 1);

    GtkWidget *manual_clock_radio = gtk_radio_button_new_with_label(NULL, "Manual");
    gtk_grid_attach(GTK_GRID(grid), manual_clock_radio, 2, 5, 1, 1);
    g_signal_connect(manual_clock_radio, "toggled", G_CALLBACK(clock_type_changed), NULL);

    GtkWidget *auto_clock_radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(manual_clock_radio), "Automatic");
    gtk_grid_attach(GTK_GRID(grid), auto_clock_radio, 3, 5, 1, 1);

    // Start Button
    start_button = gtk_button_new_with_label("Start Simulation");
    gtk_grid_attach(GTK_GRID(grid), start_button, 0, 6, 4, 1);
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_simulation), NULL);

    // Process List (Moved to the bottom)
    GtkWidget *process_label = gtk_label_new("Process Queue:");
    gtk_grid_attach(GTK_GRID(grid), process_label, 0, 7, 1, 1);

    process_list = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(process_list), FALSE);
    gtk_grid_attach(GTK_GRID(grid), process_list, 0, 8, 4, 1);

    // Show all widgets
    gtk_widget_show_all(window);

    // Enter the GTK main loop
    gtk_main();

    return 0;
}