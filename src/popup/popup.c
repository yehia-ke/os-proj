#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

char* show_text_entry_popup(const char *prompt) {
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    int response;

    // Create a dialog window
    dialog = gtk_dialog_new_with_buttons(
        "Input Dialog",
        NULL,
        flags,
        "OK", GTK_RESPONSE_OK,
        NULL
    );

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 105);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);       
    GtkWidget *action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    gtk_widget_set_halign(action_area, GTK_ALIGN_CENTER);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(prompt);
    entry = gtk_entry_new();

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_container_add(GTK_CONTAINER(content_area), entry);

    gtk_widget_show_all(dialog);

    char *input_text = NULL;

    do {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

        if (response == GTK_RESPONSE_OK && strlen(text) > 0) {
            input_text = strdup(text);
            break;
        }

        GtkWidget *warning_dialog = gtk_message_dialog_new(
            GTK_WINDOW(dialog),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please enter a value before proceeding."
        );
        gtk_dialog_run(GTK_DIALOG(warning_dialog));
        gtk_widget_destroy(warning_dialog);
    } while (1);

    gtk_widget_destroy(dialog);

    return input_text; 
}