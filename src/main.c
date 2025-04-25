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

GtkWidget *window;
GtkWidget *fixed1;
GtkWidget *processWindow;
GtkWidget *processTree;
GtkWidget *processStore;
GtkBuilder *builder;

void create_columns();

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // Load the UI from the Glade file
    builder = gtk_builder_new_from_file("./glade/pt1.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
    processWindow = GTK_WIDGET(gtk_builder_get_object(builder, "processWindow"));
    processTree = GTK_WIDGET(gtk_builder_get_object(builder, "processTree"));   
    processStore = GTK_WIDGET(gtk_builder_get_object(builder, "processStore")); 
    create_columns();

    // Show the main window
    gtk_widget_show(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

void create_columns(){

 
     // Add column headers
     GtkCellRenderer *renderer;
     GtkTreeViewColumn *column;
 
     // Column 1: Process ID
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Process ID", renderer, "text", 0, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
 
     // Column 2: Process State
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Process State", renderer, "text", 1, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
 
     // Column 3: Current Priority
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Current Priority", renderer, "text", 2, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
 
     // Column 4: Memory Lower Bound
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Memory Lower Bound", renderer, "text", 3, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
 
     // Column 5: Memory Upper Bound
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Memory Upper Bound", renderer, "text", 4, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
 
     // Column 6: Program Counter
     renderer = gtk_cell_renderer_text_new();
     column = gtk_tree_view_column_new_with_attributes("Program Counter", renderer, "text", 5, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(processTree), column);
}