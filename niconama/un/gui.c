#include "gui.h"

static GtkWidget* create_list_model(void){
	GtkWidget*treeview;
	GtkListStore*liststore;
	GtkCellRenderer*renderer;
	GtkTreeViewColumn*column;

	liststore=gtk_list_store_new(N_COLUMNS,G_TYPE_UINT,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
	treeview=gtk_tree_view_new_with_model(GTK_TREE_MODEL(liststore));
	g_object_unref(liststore);

	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("No.",renderer,"text",COLUMN_NUM,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview),column);
	column=gtk_tree_view_column_new_with_attributes("Time",renderer,"text",COLUMN_TIME,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview),column);
	column=gtk_tree_view_column_new_with_attributes("Name",renderer,"text",COLUMN_NAME,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview),column);
	column=gtk_tree_view_column_new_with_attributes("Comment",renderer,"text",COLUMN_CHAT,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview),column);
	return treeview;
}

void *guimain(void*var) {
	GtkWidget*window;
	GtkWidget*treeview;
	GtkWidget*scrolled_window;

	gtk_init(NULL,NULL);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"ListExample");
	gtk_container_set_border_width(GTK_CONTAINER(window),5);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_set_size_request(window,1280,720);
	
	scrolled_window=gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(window), scrolled_window);

	treeview=create_list_model();
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),treeview);
	
	*((GtkListStore**)var) = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}