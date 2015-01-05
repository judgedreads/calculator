#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


// This function parses the math string and evaluates it as math
// by calling the above functions.
// To handle multiple operations, need a function to split expression
// into array of form [num, op, num, op, num, ...]. Then can loop over
// that array to apply BIDMAS and calculate the result.

int calculate(int x, int y, char op)
{
    if(op == '+') {
        return x + y;
    } else if(op == '-') {
        return x - y;
    } else if(op == '*') {
        return x * y;
    } else if(op == '/') {
        return x / y;
    }

    // Reaching here means no valid operator was found - an error
    return -1;
}

//A recursive function to parse the math expression and evaluate it.
char *parseMath(const char *s, char *result)
{
    // scan the first operation and operands from the expression
    int x;
    int y;
    char op;

    // We know the expected format of the input string: int|op|int...
    sscanf(s, "%d%c%d", &x, &op, &y);

    // Find the offset for the rest of the expression and point to it
    int offset = snprintf(NULL, 0, "%d%c%d", x, op, y);
    const char *rest = s + offset;

    // evaluate the first operation
    int temp = calculate(x, y, op);

    // join the result back to the rest of the expression
    sprintf(result, "%d%s", temp, rest);

    // recursive statement
    if(rest[0] == '\0') {
        return result;
    } else {
        return parseMath(result, result);
    }

}

// Callback for the calculator buttons
static void on_button_press(GtkWidget *widget, gpointer buffer)
{
    /* 
       The (char *) is not necessary (in C) but deals with quirky
       architectures that can change the length of void pointers
       to less than a char pointer. The code will compile and
       run without this but the compiler will complain. This is
       because gpointer is a pointer to void and (char *) just
       converts a void pointer to a char pointer.
       */
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));
    //printf("Button %s pressed.\n", label);
    // label is a pointer so need to evaluate the thing it's pointing
    // at i.e. *label
    if(*label == '=') {
        const char *exp = gtk_entry_buffer_get_text(buffer);
        char *result = malloc(sizeof(int)*8/3 + 2 + strlen(exp));
        char *ans = parseMath(exp, result);
        
        gtk_entry_buffer_set_text(buffer, ans, -1);

        assert(result != NULL);
        free(result);
    } else if(*label == 'C') {
        gtk_entry_buffer_delete_text(buffer, 0, -1);
    } else {
    guint len_buff = gtk_entry_buffer_get_length(buffer);
    gtk_entry_buffer_insert_text(buffer, len_buff, label, -1);
    }
}

// Optional delete signal handler
static gboolean on_delete_event(GtkWidget *widget,
        GdkEvent *event, gpointer data)
{
    /*
       Returning FALSE would send the "destroy" signal, TRUE
       would not. This is useful to display warnings about
       closing.
       */

    printf("delete event occurred\n");

    return FALSE;
}


int main(int argc, char *argv[])
{
    // set up GTK 'objects' to use
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *grid;
    GtkWidget *entry;
    GtkEntryBuffer *buffer;

    // necessary gtk init method
    gtk_init(&argc, &argv);

    // create a new window, set title, and connect delete/destroy signals
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    g_signal_connect(window, "delete-event", G_CALLBACK(on_delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // set border width
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // create a grid and add it to the window
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // This is a buffer to hold the  text for the text view widget
    buffer = gtk_entry_buffer_new(NULL, -1);

    // an array to hold the button labels so they can be easily looped over
    char *button_labels[] = {
        "7", "8", "9", "+",
        "4", "5", "6", "-",
        "1", "2", "3", "*",
        "0", "=", "C", "/"
    };
    int num_labels = 16;
    int i = 0;

    // loop to create a button for each label in the array above, connect
    // it to the callback, and add it to the grid.
    for(i = 0; i < num_labels; i++) {
        int j = i / 4;

        button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_press), buffer);
        gtk_grid_attach(GTK_GRID(grid), button, i - (4*j), j, 1, 1);
    }

    // set up the text display area
    entry = gtk_entry_new();
    gtk_entry_set_buffer(GTK_ENTRY(entry), GTK_ENTRY_BUFFER(buffer));
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 4, 4, 1);

    // show all widgets attached to the main window
    gtk_widget_show_all(window);

    // run the GTK main loop
    gtk_main();

    return 0;
}
