#ifndef ERROR_FUNCTION_H
#define ERROR_FUNCTION_H

#include <gtk/gtk.h>

// Declare the main window as an extern variable
extern GtkWidget *main_window;

// Function to show an error message dialog
void show_error_message(const char *message);

#endif 