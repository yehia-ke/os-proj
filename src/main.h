#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>
#include "./queue/queue.h"
#include "pcb/pcb.h"
#include "interpreter/interpreter.h"
#include "scheduler/scheduler.h"
#include "AutomaticClock/AutomaticClock.h"
#include "ManualClock/ManualClock.h"
#include <unistd.h>
#include <glib.h>

// Global variables for labels
extern GtkWidget *processnumlabel;
extern GtkWidget *clocklabel;
extern GtkWidget *schedulerlabel;
extern GtkWidget *userInputMutex;
extern GtkWidget *userOutputMutex;
extern GtkWidget *fileMutex;

// Global variables for tree stores
extern GtkTreeStore *blockStore;
extern GtkTreeStore *processStore;
extern GtkTreeStore *readyStore;
extern GtkTreeStore *resourceblockStore;
extern GtkTreeStore *runningStore;

// Global variables for windows
extern GtkWidget *main_window;

// Global variables for clocks
extern ManualClock *manualClock;
extern AutomaticClock *automaticClock;
extern char *clocktype;

// Function declarations
void set_processnumlabel(const char *text);
void set_clocklabel(const char *text);
void set_schedulerlabel(const char *text);
void set_userInputMutex(const char *text);
void set_userOutputMutex(const char *text);
void set_fileMutex(const char *text);

void printtogui(const char *message);
void show_error_message(const char *message);
static char *get_current_timestamp();
static gboolean console_output_callback(GIOChannel *source, GIOCondition condition, gpointer data);
static void redirect_console_to_text_view(GtkWidget *log_text_view);

void update_blockStore();
void update_processStore();
void update_readyStore();
void update_resourceblockStore();
void update_runningStore();

void on_addprocessbutton_clicked(GtkWidget *widget, gpointer data);
void on_arrivaltimeinput_activate(GtkWidget *widget, gpointer data);
void on_programfilepath_file_set(GtkWidget *widget, gpointer data);
void on_fcfs_activate(GtkWidget *widget, gpointer data);
void on_rr_activate(GtkWidget *widget, gpointer data);
void on_mlfq_activate(GtkWidget *widget, gpointer data);
void on_start_clicked(GtkWidget *widget, gpointer data);
void on_stop_clicked(GtkWidget *widget, gpointer data);
void on_reset_clicked(GtkWidget *widget, gpointer data);
void on_autoswitcher_clicked(GtkWidget *widget, gpointer data);
void on_manualstep_clicked(GtkWidget *widget, gpointer data);
void checkArrivalTime();

#endif // MAIN_H