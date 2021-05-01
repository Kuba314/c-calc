#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "token_list.h"
#include "tokenize.h"
#include "evaluate.h"
#include "error.h"

GtkWidget *input;
GtkWidget *result_label;

#define MAX_RESULT_LENGTH 32

void recalculate(GtkWidget *hmm, gpointer data) {
    (void) hmm;
    (void) data;

    char result_buf[MAX_RESULT_LENGTH];

    // init tokens
    token_list_t *tokens = tl_init();
    if(tokens == NULL) {
        snprintf(result_buf, MAX_RESULT_LENGTH, "Couldn't allocate tokenlist\n");
        goto set_result;
    }

    // tokenize
    int error = tokenize(gtk_entry_get_text(GTK_ENTRY(input)), strlen(gtk_entry_get_text(GTK_ENTRY(input))), tokens);
    if(error) {
        tl_free(tokens);
        snprintf(result_buf, MAX_RESULT_LENGTH, "Could not tokenize");
        goto set_result;
    }

    // evaluate
    token_t result;
    error = evaluate_tokenized(tokens, &result);
    if(error) {
        tl_free(tokens);
        snprintf(result_buf, MAX_RESULT_LENGTH, "Could not evaluate expression");
        goto set_result;
    }
    tl_free(tokens);

    // print result
    if(result.type == TT_REAL) {
        long double res = result.data.d;
        if(res == (long double) (long long) res)
            snprintf(result_buf, MAX_RESULT_LENGTH, "%lld\n", (long long) result.data.d);
        else
            snprintf(result_buf, MAX_RESULT_LENGTH, "%Lg\n", result.data.d);
    } else if(result.type == TT_COMPLEX) {
        snprintf(result_buf, MAX_RESULT_LENGTH, "%Lf %c %Lfi\n", creall(result.data.c), (cimagl(result.data.c) < 0) ? '-' : '+', fabsl(cimagl(result.data.c)));
    } else {
        snprintf(result_buf, MAX_RESULT_LENGTH, "Mathlib returned nan result: %d\n", result.type);
    }

set_result:
    gtk_label_set_text(GTK_LABEL(result_label), result_buf);
}

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    input = gtk_entry_new();
    g_signal_connect(input, "changed", G_CALLBACK(recalculate), NULL);
    gtk_grid_attach(GTK_GRID(grid), input, 0, 0, 1, 1);

    result_label = gtk_label_new("0");
    gtk_grid_attach(GTK_GRID(grid), result_label, 0, 1, 2, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
