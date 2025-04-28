#include "error.h"

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