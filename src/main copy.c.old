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
#include <gtk/gtkx.h>
#include <signal.h>
#include <unistd.h>

// Function to populate the GtkTreeStore with example data
void populate_process_store(GtkTreeStore *processStore) {
    GtkTreeIter iter;

    // Add example rows to the GtkTreeStore
    gtk_tree_store_append(processStore, &iter, NULL); // Add a new row
    gtk_tree_store_set(processStore, &iter,
                       0, "1234",          // Process ID
                       1, "Running",       // Process State
                       2, "High",          // Current Priority
                       3, "0x12345678",    // Memory Boundaries
                       4, "Counter 0x001", // Program Counter
                       -1); // Terminate the list of arguments

    gtk_tree_store_append(processStore, &iter, NULL); // Add another row
    gtk_tree_store_set(processStore, &iter,
                       0, "5678",          // Process ID
                       1, "Stopped",       // Process State
                       2, "Low",           // Current Priority
                       3, "0x87654321",    // Memory Boundaries
                       4, "Counter 0x002", // Program Counter
                       -1); // Terminate the list of arguments
}

// Function to create explicit bindings for GtkTreeViewColumns
void setup_tree_view(GtkTreeView *treeView) {
    // Ensure each GtkTreeViewColumn is explicitly bound to a model column
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    // Bind Process ID column (column 0)
    column = gtk_tree_view_get_column(treeView, 0);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);

    // Bind Process State column (column 1)
    column = gtk_tree_view_get_column(treeView, 1);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

    // Bind Current Priority column (column 2)
    column = gtk_tree_view_get_column(treeView, 2);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 2);

    // Bind Memory Boundaries column (column 3)
    column = gtk_tree_view_get_column(treeView, 3);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 3);

    // Bind Program Counter column (column 4)
    column = gtk_tree_view_get_column(treeView, 4);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 4);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkTreeStore *processStore;
    GtkTreeView *treeView;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Load the Glade file
    builder = gtk_builder_new_from_file("./glade/pt1.glade");

    // Get the main window from the Glade file
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    if (!window) {
        g_printerr("Unable to find the 'window' object in the glade file.\n");
        return 1;
    }

    // Get the GtkTreeView object
    //treeView = GTK_TREE_VIEW(gtk_builder_get_object(builder, "processTree"));


    // Access the GtkTreeStore object
    processStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "processStore"));

    if (!processStore) {
        g_printerr("Unable to find the 'processStore' object in the glade file.\n");
        return 1;
    }

    // Ensure the tree view columns are correctly set up
    //setup_tree_view(treeView);

    // Populate the GtkTreeStore with example data
    populate_process_store(processStore);

    // Connect signals defined in the Glade file
    gtk_builder_connect_signals(builder, NULL);

    // Show the window
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Free the builder
    g_object_unref(builder);

    return 0;
}

// Signal handler for when the window is closed
void on_window_destroy() {
    gtk_main_quit();
}