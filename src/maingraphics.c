/* Copyright (C) Benjamin James Read, 2022 - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Benjamin Read <benjamin-read@hotmail.co.uk>, January 2022
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include "maingraphics.h"
#include "debugmsg.h"
#include "spellcheck.h"

// static bold toggle

static gboolean bold = FALSE;

// static gtk builder used to give access to widgets in app

static GtkBuilder *builder;

// keypress handler, initially will only handle escape key to close application

static gboolean keypress_handler(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkApplication *app = data;
    if (event->keyval == GDK_KEY_Escape){
        g_application_quit(G_APPLICATION(app));
        return TRUE;
    }
    return FALSE;
}

// a helper function to check whether a tag toggle lies within gtk text iter range

static bool has_tag_toggle(GtkTextTag *tag, GtkTextIter *start, GtkTextIter *end)
{
    GtkTextIter *nextToggle = start;
    gboolean ret = gtk_text_iter_forward_to_tag_toggle(nextToggle, tag);

    // if forward to tag toggle returns true, there is a tag toggle, if not no more is required

    if (ret == TRUE)
    {
        ret = gtk_text_iter_in_range(nextToggle, start, end);
    }

    return ret;
}

// a helper function to see whether a tag is ever inactive in an iter range

static bool tag_complete(GtkTextTag *tag, GtkTextIter *start, GtkTextIter *end)
{
    bool ret = false;

    if (gtk_text_iter_has_tag(start, tag))
    {
        GtkTextIter *nextToggle = gtk_text_iter_copy(start);
        gboolean gret = gtk_text_iter_forward_to_tag_toggle(nextToggle, tag);
        if (gret)
        {
            if (!gtk_text_iter_in_range(nextToggle, start, end))
            {
                ret = true;
            }
        }
    }
    return ret;
}

// a helper function to apply a togglable tag type to an iter text range

static void applyTogTag(GtkTextBuffer *buff, GtkTextTag *tag, GtkTextIter *start, GtkTextIter *end)
{
    GtkTextTagTable *tab = gtk_text_buffer_get_tag_table(GTK_TEXT_BUFFER(buff));

    if (tag_complete(tag, start, end))
    {
        gtk_text_buffer_remove_tag(buff, tag, start, end);
    }
    else
    {
        gtk_text_buffer_apply_tag(buff, tag, start, end);
    }
}

// handles the event that the bold button is pressed

static gboolean enbolden(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "bold");

    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);

    applyTogTag(buff, tag, &start, &end);

    return TRUE;
}

// handles the event that the italics toolbutton is pressed

static gboolean italicise(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "ital");

    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);

    applyTogTag(buff, tag, &start, &end);

    return TRUE;
}

// handles the event that the underline toolbutton is pressed

static gboolean underline(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "uline");

    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);

    applyTogTag(buff, tag, &start, &end);

    return TRUE;
}

// handles the event that the strikethrough toolbutton is pressed

static gboolean strikethough(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "sthru");

    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);

    applyTogTag(buff, tag, &start, &end);

    return TRUE;
}

// handles the event that the indent toolbutton is pressed

static gboolean indent(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    const int indentIncrement = 25;
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };
    GValue IRET = { 0 };

    if (tags != NULL)
    {
        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "indent-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);
            if (tagFound)
            {
                gint indentSize;
                gchar tagName[30];

                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);

                g_object_get_property(tags->data, "indent", &IRET);
                indentSize = g_value_get_int(&IRET);
                indentSize += indentIncrement;
                sprintf(tagName, "indent%i", indentSize);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, tagName);

                if (tag == NULL)
                {
                    gtk_text_buffer_create_tag(buff, tagName, "indent", indentSize, NULL);
                    tag = gtk_text_tag_table_lookup(table, tagName);
                    gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                }
                else
                {
                    gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                }

                break;
            }

            tags = tags->next;
        }
    }

    if (!tagFound)
    {
        gtk_text_buffer_create_tag(buff, "indent25", "indent", indentIncrement, NULL);
        GtkTextTag *tag = gtk_text_tag_table_lookup(table, "indent25");
        gtk_text_buffer_apply_tag(buff, tag, &start, &end);
    }

    return TRUE;
}

// handles the clicked event for the unindent toolbutton

static gboolean unindent(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    const int indentIncrement = 25;
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };
    GValue IRET = { 0 };

    if (tags != NULL)
    {
        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "indent-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);
            if (tagFound)
            {
                gint indentSize;
                gchar tagName[30];

                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);

                g_object_get_property(tags->data, "indent", &IRET);
                indentSize = g_value_get_int(&IRET);
                indentSize -= indentIncrement;
                sprintf(tagName, "indent%i", indentSize);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, tagName);

                if (tag == NULL)
                {
                    gtk_text_buffer_create_tag(buff, tagName, "indent", indentSize, NULL);
                    tag = gtk_text_tag_table_lookup(table, tagName);
                    gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                }
                else
                {
                    gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                }

                break;
            }

            tags = tags->next;
        }
    }

    return TRUE;
}

//handles the clicked event for the right justification toolbutton

static gboolean rjust(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };

    if (tags != NULL)
    {

        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "justification-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);

            if (tagFound)
            {
                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, "rjust");
                gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                break;
            }

            tags = tags->next;
        }
    }

    if (!tagFound)
    {
        GtkTextTag *tag = gtk_text_tag_table_lookup(table, "rjust");
        gtk_text_buffer_apply_tag(buff, tag, &start, &end);
    }

    return TRUE;
}

//handles the clicked event for the left justification toolbutton

static gboolean ljust(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };

    if (tags != NULL)
    {

        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "justification-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);

            if (tagFound)
            {
                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, "ljust");
                gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                break;
            }

            tags = tags->next;
        }
    }

    if (!tagFound)
    {
        GtkTextTag *tag = gtk_text_tag_table_lookup(table, "ljust");
        gtk_text_buffer_apply_tag(buff, tag, &start, &end);
    }

    return TRUE;
}

//handles the clicked event for the center justification toolbutton

static gboolean cjust(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };

    if (tags != NULL)
    {

        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "justification-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);

            if (tagFound)
            {
                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, "cjust");
                gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                break;
            }

            tags = tags->next;
        }
    }

    if (!tagFound)
    {
        GtkTextTag *tag = gtk_text_tag_table_lookup(table, "cjust");
        gtk_text_buffer_apply_tag(buff, tag, &start, &end);
    }

    return TRUE;
}

//handles the clicked event for the center justification toolbutton

static gboolean fjust(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(buff, &start, &end);
    GSList *tags = gtk_text_iter_get_tags(&start);
    gboolean tagFound = FALSE;
    GValue BRET = { 0 };

    if (tags != NULL)
    {

        while (tags->data != NULL)
        {
            g_object_get_property(G_OBJECT(tags->data), "justification-set", &BRET);
            tagFound = g_value_get_boolean(&BRET);

            if (tagFound)
            {
                gtk_text_buffer_remove_tag(buff, GTK_TEXT_TAG(tags->data), &start, &end);
                GtkTextTag *tag = gtk_text_tag_table_lookup(table, "fjust");
                gtk_text_buffer_apply_tag(buff, tag, &start, &end);
                break;
            }

            tags = tags->next;
        }
    }

    if (!tagFound)
    {
        GtkTextTag *tag = gtk_text_tag_table_lookup(table, "fjust");
        gtk_text_buffer_apply_tag(buff, tag, &start, &end);
    }

    return TRUE;
}

//handles the clicked event for butSave by saving the entire text buffer to a file name buff.f

static gboolean saveasBuf(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    
    DEB("Registering serialisation tagset\n");
    GtkTextIter start, end;
    GdkAtom format = gtk_text_buffer_register_serialize_tagset(buff, NULL);
    DEB("Getting selection iterators\n");
    gtk_text_buffer_get_start_iter(buff, &start);
    gtk_text_buffer_get_end_iter(buff, &end);
    gsize len;
    DEB("Serializing text buffer\n");
    char *text = (char *) gtk_text_buffer_serialize(buff, buff, format, &start, &end, &len);
    DEB("%s\n", text);
    DEB("%ld\n", len);

    // open a save dialog so the user can choose where to save data

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Abspeichern...",
                      NULL,
                      GTK_FILE_CHOOSER_ACTION_SAVE,
                      "_Cancel", GTK_RESPONSE_CANCEL,
                      "_Save", GTK_RESPONSE_ACCEPT,
                      NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
    char *filename;
    gboolean result;
    GError *err = NULL;

    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

    // set contents of file to the value of 'text'

    if (filename != NULL)
        result = g_file_set_contents (filename, text, len, &err);
    else
        result = g_file_set_contents (filename, text, len, &err);

    if (result == FALSE)
    {
        printf("error\n");
    }        

    }
    
    gtk_widget_destroy (dialog);

    return TRUE;
}

//handles the clicked event for butOpen by allowing a fileto be selected and deserialized into a textbuffer

static gboolean saveBuf(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextIter start;
    gtk_text_buffer_get_start_iter(buff, &start);
    GError *err = NULL;
    
    GdkAtom format = gtk_text_buffer_register_deserialize_tagset(buff, NULL);


    return TRUE;
}

//handles the clicked event for butOpen by allowing a fileto be selected and deserialized into a textbuffer

static gboolean openFile(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buff, &end);

    gchar *loadData;
    gsize length;

    // open a load dialog so the user can choose the file to load

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Load...",
                      NULL,
                      GTK_FILE_CHOOSER_ACTION_SAVE,
                      "_Cancel", GTK_RESPONSE_CANCEL,
                      "_Save", GTK_RESPONSE_ACCEPT,
                      NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
    char *filename;
    gboolean result;
    GError *err = NULL;

    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

    // load the data from the appropriate file

    printf("%s\n", filename);
    if (filename != NULL)
    {
        g_file_get_contents(filename, &loadData, &length, &err);
        printf("%s\n", loadData);
    }
    
    // deserialize the data and put it into a text buffer

    GdkAtom format = gtk_text_buffer_register_deserialize_tagset(buff, NULL);
    gboolean ret = gtk_text_buffer_deserialize(buff, buff, format, &end, loadData, length, &err);
    printf("%i\n", ret);
    if (err)
        printf("%s\n", err->message);
    
    }
    
    gtk_widget_destroy (dialog);

    return TRUE;
}

// a callback function to be provided to clipboard request image. Called when image is received from owning process.

void pasteImageCallback(GtkClipboard* clipboard, GdkPixbuf* pixbuf, gpointer data)
{
    if (!pixbuf)
        printf("Error, paste callback received no image\n");
    else
    {
        // GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
        // GtkTextView *view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "view0"));
        // GtkTextIter cursor;
        // gtk_text_buffer_get_iter_at_mark(buff, &cursor, gtk_text_buffer_get_insert(buff));
        // gtk_text_buffer_insert_pixbuf(buff, &cursor, pixbuf);

        // some code that places an invisible button or something over the pixbuf

        // GtkTextChildAnchor *anchor = gtk_text_buffer_create_child_anchor(buff, &cursor);
        GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
        GtkTextView *view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "view0"));
        gtk_text_view_add_overlay(view, image, 0, 0);
        // gtk_text_view_add_child_at_anchor(view, image, anchor);
        // gtk_widget_show(image);
    }
    return;
}

// handles the event that an image is pasted into a textbuffer

static gboolean pasteImage(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GdkDisplay *display = gdk_display_get_default();
    GtkClipboard *clipboard = gtk_clipboard_get_default(display);
    gtk_clipboard_request_image(clipboard, pasteImageCallback, NULL);
    return TRUE;
}

// spellcheck a range of text iters

static void spellcheck_range(GtkTextBuffer *buff, GtkTextIter start, GtkTextIter end)
{
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "misspelt");
    
    GtkTextIter wstart, wend;
    wstart = end;
    gtk_text_iter_backward_word_start(&wstart);
    wend = wstart;
    gtk_text_iter_forward_word_end(&wend);
    gchar *text = gtk_text_buffer_get_text(buff, &wstart, &wend, FALSE);

    if (spellcheck_isvalidword(text))
        gtk_text_buffer_remove_tag(buff, tag, &wstart, &wend);
    else
        gtk_text_buffer_apply_tag(buff, tag, &wstart, &wend);

    while (!gtk_text_iter_is_start(&wstart))
    {
        gtk_text_iter_backward_word_start(&wstart);
        wend = wstart;
        gtk_text_iter_forward_word_end(&wend);
        gchar *text = gtk_text_buffer_get_text(buff, &wstart, &wend, FALSE);
        if (spellcheck_isvalidword(text))
            gtk_text_buffer_remove_tag(buff, tag, &wstart, &wend);
        else
            gtk_text_buffer_apply_tag(buff, tag, &wstart, &wend);
    } 
}

// spellcheck entire text buffer

static void spellcheck_all(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));
    GtkTextTagTable *table = GTK_TEXT_TAG_TABLE(gtk_builder_get_object(builder, "tab0"));
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, "misspelt");

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buff, &start);
    gtk_text_buffer_get_end_iter(buff, &end);
    
    GtkTextIter wstart, wend;
    wstart = end;
    gtk_text_iter_backward_word_start(&wstart);
    wend = wstart;
    gtk_text_iter_forward_word_end(&wend);
    gchar *text = gtk_text_buffer_get_text(buff, &wstart, &wend, FALSE);

    if (spellcheck_isvalidword(text))
        gtk_text_buffer_remove_tag(buff, tag, &wstart, &wend);
    else
        gtk_text_buffer_apply_tag(buff, tag, &wstart, &wend);

    while (!gtk_text_iter_is_start(&wstart))
    {
        gtk_text_iter_backward_word_start(&wstart);
        wend = wstart;
        gtk_text_iter_forward_word_end(&wend);
        gchar *text = gtk_text_buffer_get_text(buff, &wstart, &wend, FALSE);
        if (spellcheck_isvalidword(text))
            gtk_text_buffer_remove_tag(buff, tag, &wstart, &wend);
        else
            gtk_text_buffer_apply_tag(buff, tag, &wstart, &wend);
    } 
}

// this is the main runner function for the graphical appliation
// a callback for the activation event of the GTK app object

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *view;
    GtkWidget *butBold, *butItal, *butUline, *butSthru, *butIndent, *butUnindent, *butLjust, *butRjust, *butCjust, *butFjust, *butSaveas, *butSave, *butOpen, *butPaste;

    // create + initialise a window

    window = gtk_application_window_new(app);

    builder = gtk_builder_new_from_file("../res/editorMain.glade");
    view = GTK_WIDGET(gtk_builder_get_object(builder, "view0"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "editorMain1"));
    GtkTextBuffer *buff = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "buff0"));

    // use gtk builder to obtain a pointer to each toolbutton in the toolbar

    butBold = GTK_WIDGET(gtk_builder_get_object(builder, "butBold"));
    butItal = GTK_WIDGET(gtk_builder_get_object(builder, "butItal"));
    butUline = GTK_WIDGET(gtk_builder_get_object(builder, "butUline"));
    butSthru = GTK_WIDGET(gtk_builder_get_object(builder, "butSthru"));
    butIndent = GTK_WIDGET(gtk_builder_get_object(builder, "butIndent"));
    butUnindent = GTK_WIDGET(gtk_builder_get_object(builder, "butUnindent"));
    butCjust = GTK_WIDGET(gtk_builder_get_object(builder, "butCjust"));
    butLjust = GTK_WIDGET(gtk_builder_get_object(builder, "butLjust"));
    butRjust = GTK_WIDGET(gtk_builder_get_object(builder, "butRjust"));
    butFjust = GTK_WIDGET(gtk_builder_get_object(builder, "butFjust"));
    butSaveas = GTK_WIDGET(gtk_builder_get_object(builder, "butSaveas"));
    butOpen = GTK_WIDGET(gtk_builder_get_object(builder, "butOpen"));
    butPaste = GTK_WIDGET(gtk_builder_get_object(builder, "butPaste"));


    // connect event handlers for each toolbutton in the toolbar

    g_signal_connect(G_OBJECT(butBold), "clicked", G_CALLBACK(enbolden), app);
    g_signal_connect(G_OBJECT(butItal), "clicked", G_CALLBACK(italicise), app);
    g_signal_connect(G_OBJECT(butUline), "clicked", G_CALLBACK(underline), app);
    g_signal_connect(G_OBJECT(butSthru), "clicked", G_CALLBACK(strikethough), app);
    g_signal_connect(G_OBJECT(butIndent), "clicked", G_CALLBACK(indent), app);
    g_signal_connect(G_OBJECT(butUnindent), "clicked", G_CALLBACK(unindent), app);
    g_signal_connect(G_OBJECT(butRjust), "clicked", G_CALLBACK(rjust), app);
    g_signal_connect(G_OBJECT(butLjust), "clicked", G_CALLBACK(ljust), app);
    g_signal_connect(G_OBJECT(butCjust), "clicked", G_CALLBACK(cjust), app);
    g_signal_connect(G_OBJECT(butFjust), "clicked", G_CALLBACK(fjust), app);
    g_signal_connect(G_OBJECT(butPaste), "clicked", G_CALLBACK(pasteImage), app);

    // connect event handlers for menu bar activations

    g_signal_connect(G_OBJECT(butSaveas), "activate", G_CALLBACK(saveasBuf), app);
    g_signal_connect(G_OBJECT(butOpen), "activate", G_CALLBACK(openFile), app);

    // connect event handler for textbuffer changed event

    g_signal_connect(G_OBJECT(buff), "changed", G_CALLBACK(spellcheck_all), app);

    // set up text tag table with tag types

    gtk_text_buffer_create_tag(buff, "bold", "weight", 700, NULL);
    gtk_text_buffer_create_tag(buff, "ital", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(buff, "uline", "underline", PANGO_UNDERLINE_SINGLE, NULL);
    gtk_text_buffer_create_tag(buff, "sthru", "strikethrough", true, NULL);

    // set up text tags for justification, only one should be active on some text at a time

    gtk_text_buffer_create_tag(buff, "ljust", "justification", GTK_JUSTIFY_LEFT, NULL);
    gtk_text_buffer_create_tag(buff, "rjust", "justification", GTK_JUSTIFY_RIGHT, NULL);
    gtk_text_buffer_create_tag(buff, "cjust", "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_buffer_create_tag(buff, "fjust", "justification", GTK_JUSTIFY_FILL, NULL);

    // add css provider for main text editor, this includes toolbutton styles

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "../res/buk.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // enable keypress on the window

    gtk_widget_add_events(GTK_WIDGET(window), GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(keypress_handler), app);

    // install a handler to destroy the application when the window is destroyed

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(GTK_WIDGET(window));
    //test();
}

int launchGraphics(int argc, char **argv)
{
    int ret;
    GtkApplication *app;

    app = gtk_application_new("in.Buk", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    ret = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return ret;
}