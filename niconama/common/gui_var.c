#include <gtk/gtk.h>
#include "data_exchange.h"

GtkWidget *mainWindow, *mainBox, *mainToolbar;
GtkToolItem *mainToolbarNextReserve,
	*mainToolbarDoReserve,
	*mainToolbarFFmpegResend,
	*mainToolbarPreset;
GtkWidget *mainNotebook, *mainToolbarNextReserveLabel,
	*mainToolbarDoReserveLabel,
	*mainToolbarFFmpegResendLabel,
	*mainToolbarNextReserveButton,
	*mainToolbarDoReserveButton,
	*mainToolbarFFmpegResendButton,
	*mainToolbarPresetList,
	*commentEB, *commentL,
	*commentBox, *commentHB1,
	*commentCurrentEB, *commentCurrentL,
	*commentConnectedList, *commentLiveIDEdit,
	*commentConnectButton, *commentConnectButtonLabel,
	*commentDisconnect, *commentDisconnectLabel,
	*commentMain, *commentHB2,
	*commentGetOlder, *commentGetOlderLabel,
	*commentUsersList, *commentUsersListLabel,
	*commentHB3, *commentVB1,
	*commentNameEdit, *commentCommandEdit,
	*commentResetCommand, *commentResetCommandLabel,
	*commentChatEdit, *commentVB3, *commentSendNormal,
	*commentSendNormalLabel, *commentSendOperationLabel,
	*commentSendOperation, *commentSendBSP,
	*ffmpegEB, *ffmpegL, *commentSendBSPLabel,
	*ffmpegBox, *ffmpegLog, *ffmpegHB1, *ffmpegStart,
	*ffmpegStop, *alertEB, *alertL, *alertBox,
	*ffmpegStartLabel, *ffmpegStopLabel,
	*alertFavorites, *alertVB1,
	*alertIDEdit, *alertIDType,
	*alertAdd, *alertDelete, *alertAddLabel,
	*alertStatus, *alertDeleteLabel,
	*alertServerName, *alertServerPort,
	*alertServerThread, *alertLastUpdateTime,
	*alertUserID, *alertConnect, *alertConnectLabel,
	*alertDisconnect, *multimediaEB, *alertDisconnectLabel,
	*multimediaL, *multimediaBox,
	*multimediaHB1, *multimediaPreview,
	*multimediaVideoSourceList,
	*multimediaVideoMenuBar,
	*multimediaVideoMenuBarAdd,
	*multimediaVideoMenuBarAddLabel,
	*multimediaVideoMenuBarAddMenu,
	*multimediaVideoMenuBarDelete,
	*multimediaVideoMenuBarDeleteLabel,
	*multimediaVB1, *multimediaPresetList,
	*multimediaPresetName, *multimediaPresetAdd,
	*multimediaPresetDelete, *multimediaHB2,
	*multimediaPresetAddLabel, *multimediaPresetDeleteLabel,
	*multimediaAudioMuxerBox, *multimediaVB2,
	*multimediaAudioAdd, *multimediaAudioDelete,
	*multimediaAudioAddLabel, *multimediaAudioDeleteLabel,
	*multimediaAdvancedSettings,
	*multimediaAdvancedSettingsLabel,
	*reserveEB, *reserveL,
	*reserveBox, *reserveVB1,
	*reservePresetList, *reservePresetName,
	*reservePresetAdd, *reservePresetDelete,
	*reservePresetAddLabel, *reservePresetDeleteLabel,
	*reserveNextNotify, *reserveNextNotifyEdit,
	*reserveTitle, *reserveVB2,
	*reserveDescription, *reserveHB1,
	*reserveCommunityList, *reserveVB3,
	*reserveCategoryList, *reserveFace,
	*reserveCall, *reserveCruise,
	*reserveVB4, *reserveCommunityOnly,
	*reserveTimeshift, *reserveAdsense,
	*reserveIchiba, *reserveTwitterBox,
	*reserveTwitter, *reserveTwitterTags,
	*reserveTag0, *reserveTag1, *reserveTB3,
	*reserveTag2, *reserveTB1, *reserveTB4,
	*reserveTag3, *reserveTag4, *reserveTB2,
	*reserveTag5, *reserveTag6, *reserveTB5,
	*reserveTag7, *reserveTag8,
	*reserveTag9, *reserveMusic, *reserveMusicLabel,
	*commentSettingEB, *commentSettingL,
	*commentSettingBox, *commentSettingHB1, *commentSettingNGEdit,
	*commentSettingNGUsers, *commentSettingNGUsersBox,
	*commentSettingNGComments, *commentSettingNGCommentsBox,
	*commentSettingNGUsersLabel, *commentSettingNGCommentsLabel,
	*commentSettingNGUsersAdd, *commentSettingNGUsersAddLabel,
	*commentSettingNGUsersDelete, *commentSettingNGUsersDeleteLabel,
	*commentSettingNGCommentsAdd, *commentSettingNGCommentsAddLabel,
	*commentSettingNGCommentsDelete, *commentSettingNGCommentsDeleteLabel,
	*commentSettingResponseList, *commentSettingResponseListLabel,
	*commentSettingHB5, *commentSettingVB2,
	*commentSettingQuestion, *commentSettingQuestionLabel,
	*commentSettingResponse, *commentSettingResponseLabel,
	*commentSettingQuestionBox, *commentSettingResponseBox,
	*commentSettingVB3, *commentSettingResponseDeleteLabel,
	*commentSettingResponseAdd, *commentSettingResponseAddLabel,
	*commentSettingResponseDelete,
	*handleNameEB, *handleNameL, *handleName184Delete,
	*handleNameBox, *handleNameHB1, *handleName184B,
	*handleNameRawList, *handleNameRawB, *handleName184Label,
	*handleName184Season, *handleName184List, *handleNameRawLabel,
	*handleNameVB2, *handleNameIDLabel, *handleNameHandleLabel,
	*handleNameID, *handleNameHandle, *handleName184DeleteLabel,
	*handleNameAdd, *handleNameRawDeleteLabel,
	*handleNameRawDelete, *handleNameAddLabel;
GtkListStore *commentConnectedListStore,
	*commentMainStore, *alertFavoritesStore,
	*alertIDTypeStore, *multimediaVideoSourceListStore,
	*multimediaPresetListStore, *reservePresetListStore,
	*reserveCommunityListStore, *reserveCategoryListStore,
	*commentSettingNGUsersStore, *commentSettingNGCommentsStore,
	*commentSettingResponseListStore,
	*handleNameRawListStore, *handleName184ListStore;
extern THREAD_RET_TYPE start_streaming_thread(void *dummy);
static gboolean start_streaming(GtkWidget *widget, GdkEventKey *key,
									gpointer user_data) {
	stop_flag = FALSE;
	THREAD_CREATE_WIN2(start_streaming_thread, NULL);
	return FALSE;
}
static gboolean stop_streaming(GtkWidget *widget, GdkEventKey *key,
									gpointer user_data) {
	stop_flag = TRUE;
	return FALSE;
}
void createMainGUI(){
	gtk_init(NULL,NULL);
	// Create instances
	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	mainBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	mainToolbar=gtk_toolbar_new();
	mainToolbarNextReserve=gtk_tool_item_new();
	mainToolbarNextReserveButton=gtk_toggle_button_new();
	mainToolbarNextReserveLabel=gtk_label_new("次枠取り");
	mainToolbarDoReserve=gtk_tool_item_new();
	mainToolbarDoReserveButton=gtk_button_new();
	mainToolbarDoReserveLabel=gtk_label_new("枠取り実行");
	mainToolbarFFmpegResend=gtk_tool_item_new();
	mainToolbarFFmpegResendButton=gtk_button_new();
	mainToolbarFFmpegResendLabel=gtk_label_new("映像再送");
	mainToolbarPreset=gtk_tool_item_new();
	mainToolbarPresetList=gtk_combo_box_new();
	mainNotebook=gtk_notebook_new();
	commentEB=gtk_event_box_new();
	commentL=gtk_label_new("コメント");
	commentBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentCurrentEB=gtk_event_box_new();
	commentCurrentL=gtk_label_new("未接続");
	commentConnectedList=gtk_combo_box_new();
	commentConnectedListStore=gtk_list_store_new(1,G_TYPE_STRING);
	commentLiveIDEdit=gtk_entry_new();
	commentConnectButton=gtk_button_new();
	commentConnectButtonLabel=gtk_label_new("接続");
	commentDisconnect=gtk_button_new();
	commentDisconnectLabel=gtk_label_new("切断");
	commentMain=gtk_tree_view_new();
	commentMainStore=gtk_list_store_new(1,G_TYPE_STRING);//TODO
	commentHB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentGetOlder=gtk_button_new();
	commentGetOlderLabel=gtk_label_new("更に古いコメントを取得");
	commentUsersList=gtk_button_new();
	commentUsersListLabel=gtk_label_new("書き込みユーザー一覧");

	commentHB3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentNameEdit=gtk_entry_new();
	commentCommandEdit=gtk_entry_new();
	commentResetCommand=gtk_button_new();
	commentResetCommandLabel=gtk_label_new("/reset");
	commentChatEdit=gtk_text_view_new();
	commentVB3=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSendNormal=gtk_button_new();
	commentSendNormalLabel=gtk_label_new("通常コメントとして投稿");
	commentSendOperation=gtk_button_new();
	commentSendOperationLabel=gtk_label_new("運営コメントとして投稿");
	commentSendBSP=gtk_button_new();
	commentSendBSPLabel=gtk_label_new("BSPコメントとして投稿");
	ffmpegEB=gtk_event_box_new();
	ffmpegL=gtk_label_new("FFmpeg");
	ffmpegBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	ffmpegLog=gtk_text_view_new();
	ffmpegHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	ffmpegStart=gtk_button_new();
	ffmpegStartLabel=gtk_label_new("開始・再送");
	g_signal_connect(G_OBJECT(ffmpegStart),"clicked",
		G_CALLBACK(start_streaming),NULL);
	ffmpegStop=gtk_button_new();
	ffmpegStopLabel=gtk_label_new("停止");
	g_signal_connect(G_OBJECT(ffmpegStop),"clicked",
		G_CALLBACK(stop_streaming),NULL);
	alertEB=gtk_event_box_new();
	alertL=gtk_label_new("アラート");
	alertBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	alertFavorites=gtk_tree_view_new();
	alertFavoritesStore=gtk_list_store_new(1,G_TYPE_STRING);
	alertVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	alertIDEdit=gtk_entry_new();
	alertIDType=gtk_combo_box_new();
	alertIDTypeStore=gtk_list_store_new(1,G_TYPE_STRING);
	alertAdd=gtk_button_new();
	alertAddLabel=gtk_label_new("追加");
	alertDelete=gtk_button_new();
	alertDeleteLabel=gtk_label_new("削除");
	alertStatus=gtk_label_new("未接続");
	alertServerName=gtk_label_new(NULL);
	alertServerPort=gtk_label_new(NULL);
	alertServerThread=gtk_label_new(NULL);
	alertLastUpdateTime=gtk_label_new(NULL);
	alertUserID=gtk_label_new(NULL);
	alertConnect=gtk_button_new();
	alertConnectLabel=gtk_label_new("接続");
	alertDisconnect=gtk_button_new();
	alertDisconnectLabel=gtk_label_new("切断");
	multimediaEB=gtk_event_box_new();
	multimediaL=gtk_label_new("マルチメディア");
	multimediaBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	multimediaHB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	multimediaPreview=gtk_image_new();
	multimediaVideoSourceList=gtk_tree_view_new();
	multimediaVideoSourceListStore=gtk_list_store_new(1,G_TYPE_STRING);//TODO
	multimediaVideoMenuBar=gtk_menu_bar_new();
	multimediaVideoMenuBarAdd=gtk_menu_item_new();
	multimediaVideoMenuBarAddLabel=gtk_label_new("追加");
	multimediaVideoMenuBarAddMenu=gtk_menu_new();


	multimediaVideoMenuBarDelete=gtk_menu_item_new();
	multimediaVideoMenuBarDeleteLabel=gtk_label_new("削除");
	multimediaVB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	multimediaPresetList=gtk_combo_box_new();
	multimediaPresetListStore=gtk_list_store_new(1,G_TYPE_STRING);
	multimediaPresetName=gtk_entry_new();
	multimediaPresetAdd=gtk_button_new();
	multimediaPresetAddLabel=gtk_label_new("追加");
	multimediaPresetDelete=gtk_button_new();
	multimediaPresetDeleteLabel=gtk_label_new("削除");
	multimediaHB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	multimediaAudioMuxerBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);

	multimediaVB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	multimediaAudioAdd=gtk_button_new();
	multimediaAudioAddLabel=gtk_label_new("追加");
	multimediaAudioDelete=gtk_button_new();
	multimediaAudioDeleteLabel=gtk_label_new("削除");
	multimediaAdvancedSettings=gtk_frame_new(NULL);
	multimediaAdvancedSettingsLabel=gtk_label_new("拡張設定");
	reserveEB=gtk_event_box_new();
	reserveL=gtk_label_new("枠取り");
	reserveBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveVB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reservePresetList=gtk_combo_box_new();
	reservePresetListStore=gtk_list_store_new(1,G_TYPE_STRING);
	reservePresetName=gtk_entry_new();
	reservePresetAdd=gtk_button_new();
	reservePresetAddLabel=gtk_label_new("追加");
	reservePresetDelete=gtk_button_new();
	reservePresetDeleteLabel=gtk_label_new("削除");
	reserveNextNotify=gtk_check_button_new_with_label("次枠通知");
	reserveNextNotifyEdit=gtk_entry_new();
	reserveTitle=gtk_entry_new();
	reserveVB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveDescription=gtk_text_view_new();
	reserveHB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveCommunityList=gtk_combo_box_new();
	reserveCommunityListStore=gtk_list_store_new(1,G_TYPE_STRING);
	reserveVB3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveCategoryList=gtk_combo_box_new();
	reserveCategoryListStore=gtk_list_store_new(1,G_TYPE_STRING);
	reserveFace=gtk_check_button_new_with_label("顔出し");
	reserveCall=gtk_check_button_new_with_label("凸待ち");
	reserveCruise=gtk_check_button_new_with_label("クルーズ待ち");
	reserveVB4=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveCommunityOnly=gtk_check_button_new_with_label("コミュ限");
	reserveTimeshift=gtk_check_button_new_with_label("タイムシフト");
	reserveAdsense=gtk_check_button_new_with_label("広告");
	reserveIchiba=gtk_check_button_new_with_label("市場");
	reserveTwitterBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTwitter=gtk_check_button_new_with_label("Twitter");
	reserveTwitterTags=gtk_entry_new();
	reserveTB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTB3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTB4=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTB5=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveTag0=gtk_entry_new();
	reserveTag1=gtk_entry_new();
	reserveTag2=gtk_entry_new();
	reserveTag3=gtk_entry_new();
	reserveTag4=gtk_entry_new();
	reserveTag5=gtk_entry_new();
	reserveTag6=gtk_entry_new();
	reserveTag7=gtk_entry_new();
	reserveTag8=gtk_entry_new();
	reserveTag9=gtk_entry_new();
	reserveMusic=gtk_button_new();
	reserveMusicLabel=gtk_label_new("使用楽曲");
	commentSettingEB=gtk_event_box_new();
	commentSettingL=gtk_label_new("コメント設定");
	commentSettingBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingNGUsersBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingNGUsersLabel=gtk_label_new("NGユーザー");
	commentSettingNGUsers=gtk_tree_view_new();
	commentSettingNGUsersStore=gtk_list_store_new(1,G_TYPE_STRING);
	commentSettingNGUsersDelete=gtk_button_new();
	commentSettingNGUsersDeleteLabel=gtk_label_new("削除");
	commentSettingNGUsersAdd=gtk_button_new();
	commentSettingNGUsersAddLabel=gtk_label_new("追加");
	commentSettingNGCommentsBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingNGCommentsLabel=gtk_label_new("NGコメント");
	commentSettingNGComments=gtk_tree_view_new();
	commentSettingNGCommentsStore=gtk_list_store_new(1,G_TYPE_STRING);
	commentSettingNGCommentsDelete=gtk_button_new();
	commentSettingNGCommentsDeleteLabel=gtk_label_new("削除");
	commentSettingNGCommentsAdd=gtk_button_new();
	commentSettingNGCommentsAddLabel=gtk_label_new("追加");
	commentSettingNGEdit=gtk_entry_new();
	commentSettingResponseListLabel=gtk_label_new("コメント自動応答・置き換え");
	commentSettingResponseList=gtk_tree_view_new();
	commentSettingResponseListStore=gtk_list_store_new(1,G_TYPE_STRING);//TODO
	commentSettingHB5=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingVB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingQuestionBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingQuestionLabel=gtk_label_new("検出文字列");
	commentSettingQuestion=gtk_entry_new();
	commentSettingResponseBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingResponseLabel=gtk_label_new("応答・置換文字列");
	commentSettingResponse=gtk_entry_new();

	commentSettingVB3=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingResponseAdd=gtk_button_new();
	commentSettingResponseAddLabel=gtk_label_new("追加");
	commentSettingResponseDelete=gtk_button_new();
	commentSettingResponseDeleteLabel=gtk_label_new("削除");
	handleNameEB=gtk_event_box_new();
	handleNameL=gtk_label_new("コテハン");
	handleNameBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleNameHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	handleNameRawB=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleNameRawLabel=gtk_label_new("生ID");
	handleNameRawList=gtk_tree_view_new();
	handleNameRawListStore=gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
	handleNameRawDelete=gtk_button_new();
	handleNameRawDeleteLabel=gtk_label_new("削除");
	handleName184B=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleName184Label=gtk_label_new("184");
	handleName184Season=gtk_label_new(NULL);
	handleName184List=gtk_tree_view_new();
	handleName184ListStore=gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
	handleName184Delete=gtk_button_new();
	handleName184DeleteLabel=gtk_label_new("削除");
	handleNameVB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	handleNameID=gtk_entry_new();
	handleNameIDLabel=gtk_label_new("ID");
	handleNameHandle=gtk_entry_new();
	handleNameHandleLabel=gtk_label_new("コテハン");
	handleNameAdd=gtk_button_new();
	handleNameAddLabel=gtk_label_new("追加");

	gtk_container_add(GTK_CONTAINER(mainWindow),mainBox);
	gtk_box_pack_start(GTK_BOX(mainBox),mainToolbar,FALSE,FALSE,0);
	gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar),mainToolbarNextReserve,0);
	gtk_container_add(GTK_CONTAINER(mainToolbarNextReserve),mainToolbarNextReserveButton);
	gtk_container_add(GTK_CONTAINER(mainToolbarNextReserveButton),mainToolbarNextReserveLabel);
	gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar),mainToolbarDoReserve,1);
	gtk_container_add(GTK_CONTAINER(mainToolbarDoReserve),mainToolbarDoReserveButton);
	gtk_container_add(GTK_CONTAINER(mainToolbarDoReserveButton),mainToolbarDoReserveLabel);
	gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar),mainToolbarFFmpegResend,2);
	gtk_container_add(GTK_CONTAINER(mainToolbarFFmpegResend),mainToolbarFFmpegResendButton);
	gtk_container_add(GTK_CONTAINER(mainToolbarFFmpegResendButton),mainToolbarFFmpegResendLabel);
	gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar),mainToolbarPreset,3);
	gtk_container_add(GTK_CONTAINER(mainToolbarPreset),mainToolbarPresetList);
	gtk_combo_box_set_model(GTK_COMBO_BOX(mainToolbarPresetList),GTK_TREE_MODEL(multimediaPresetListStore));
	gtk_box_pack_start(GTK_BOX(mainBox),mainNotebook,TRUE,TRUE,0);

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),commentBox,commentL);
	//gtk_container_add(GTK_CONTAINER(commentEB),commentL);
	gtk_box_pack_start(GTK_BOX(commentBox),commentHB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentHB1),commentCurrentEB,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentCurrentEB),commentCurrentL);
	gtk_box_pack_start(GTK_BOX(commentHB1),commentConnectedList,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(commentConnectedList),GTK_TREE_MODEL(commentConnectedListStore));
	gtk_box_pack_start(GTK_BOX(commentHB1),commentLiveIDEdit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentHB1),commentConnectButton,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentConnectButton),commentConnectButtonLabel);
	gtk_box_pack_start(GTK_BOX(commentHB1),commentDisconnect,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentDisconnect),commentDisconnectLabel);
	gtk_box_pack_start(GTK_BOX(commentBox),commentMain,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(commentMain),GTK_TREE_MODEL(commentMainStore));
	gtk_box_pack_start(GTK_BOX(commentBox),commentHB2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentHB2),commentGetOlder,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentGetOlder),commentGetOlderLabel);
	gtk_box_pack_start(GTK_BOX(commentHB2),commentUsersList,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentUsersList),commentUsersListLabel);
	gtk_box_pack_start(GTK_BOX(commentBox),commentHB3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentHB3),commentVB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentVB1),commentNameEdit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentVB1),commentCommandEdit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentVB1),commentResetCommand,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentResetCommand),commentResetCommandLabel);
	gtk_box_pack_start(GTK_BOX(commentHB3),commentChatEdit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentHB3),commentVB3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentVB3),commentSendNormal,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSendNormal),commentSendNormalLabel);
	gtk_box_pack_start(GTK_BOX(commentVB3),commentSendOperation,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSendOperation),commentSendOperationLabel);
	gtk_box_pack_start(GTK_BOX(commentVB3),commentSendBSP,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSendBSP),commentSendBSPLabel);

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),ffmpegBox,ffmpegL);
	//gtk_container_add(GTK_CONTAINER(ffmpegEB),ffmpegL);
	gtk_box_pack_start(GTK_BOX(ffmpegBox),ffmpegLog,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(ffmpegBox),ffmpegHB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(ffmpegHB1),ffmpegStart,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(ffmpegStart),ffmpegStartLabel);
	gtk_box_pack_start(GTK_BOX(ffmpegHB1),ffmpegStop,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(ffmpegStop),ffmpegStopLabel);

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),alertBox,alertL);
	//gtk_container_add(GTK_CONTAINER(alertEB),alertL);
	gtk_box_pack_start(GTK_BOX(alertBox),alertFavorites,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(alertFavorites),GTK_TREE_MODEL(alertFavoritesStore));
	gtk_box_pack_start(GTK_BOX(alertBox),alertVB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertIDEdit,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertIDType,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(alertIDType),GTK_TREE_MODEL(alertIDTypeStore));
	gtk_box_pack_start(GTK_BOX(alertVB1),alertAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(alertAdd),alertAddLabel);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(alertDelete),alertDeleteLabel);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertStatus,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertServerName,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertServerPort,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertServerThread,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertLastUpdateTime,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertUserID,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertConnect,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(alertConnect),alertConnectLabel);
	gtk_box_pack_start(GTK_BOX(alertVB1),alertDisconnect,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(alertDisconnect),alertDisconnectLabel);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),multimediaBox,multimediaL);
	//gtk_container_add(GTK_CONTAINER(multimediaEB),multimediaL);
	gtk_box_pack_start(GTK_BOX(multimediaBox),multimediaHB1,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(multimediaHB1),multimediaPreview,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(multimediaHB1),multimediaVideoSourceList,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(multimediaVideoSourceList),GTK_TREE_MODEL(multimediaVideoSourceListStore));
	gtk_box_pack_start(GTK_BOX(multimediaHB1),multimediaVideoMenuBar,FALSE,FALSE,0);
	gtk_menu_shell_append(GTK_MENU_SHELL(multimediaVideoMenuBar),multimediaVideoMenuBarAdd);
	gtk_container_add(GTK_CONTAINER(multimediaVideoMenuBarAdd),multimediaVideoMenuBarAddLabel);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(multimediaVideoMenuBarAdd),multimediaVideoMenuBarAddMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(multimediaVideoMenuBar),multimediaVideoMenuBarDelete);
	gtk_container_add(GTK_CONTAINER(multimediaVideoMenuBarDelete),multimediaVideoMenuBarDeleteLabel);
	gtk_box_pack_start(GTK_BOX(multimediaHB1),multimediaVB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(multimediaVB1),multimediaPresetList,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(multimediaPresetList),GTK_TREE_MODEL(multimediaPresetListStore));
	gtk_box_pack_start(GTK_BOX(multimediaVB1),multimediaPresetName,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(multimediaVB1),multimediaPresetAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(multimediaPresetAdd),multimediaPresetAddLabel);
	gtk_box_pack_start(GTK_BOX(multimediaVB1),multimediaPresetDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(multimediaPresetDelete),multimediaPresetDeleteLabel);
	gtk_box_pack_start(GTK_BOX(multimediaBox),multimediaHB2,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(multimediaHB2),multimediaAudioMuxerBox,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(multimediaHB2),multimediaVB2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(multimediaVB2),multimediaAudioAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(multimediaAudioAdd),multimediaAudioAddLabel);
	gtk_box_pack_start(GTK_BOX(multimediaVB2),multimediaAudioDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(multimediaAudioDelete),multimediaAudioDeleteLabel);
	gtk_box_pack_start(GTK_BOX(multimediaHB2),multimediaAdvancedSettings,FALSE,FALSE,0);
	gtk_frame_set_label_widget(GTK_FRAME(multimediaAdvancedSettings),multimediaAdvancedSettingsLabel);

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),reserveBox,reserveL);
	//gtk_container_add(GTK_CONTAINER(reserveEB),reserveL);
	gtk_box_pack_start(GTK_BOX(reserveBox),reserveVB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB1),reservePresetList,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(reservePresetList),GTK_TREE_MODEL(reservePresetListStore));
	gtk_box_pack_start(GTK_BOX(reserveVB1),reservePresetName,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB1),reservePresetAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(reservePresetAdd),reservePresetAddLabel);
	gtk_box_pack_start(GTK_BOX(reserveVB1),reservePresetDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(reservePresetDelete),reservePresetDeleteLabel);
	gtk_box_pack_start(GTK_BOX(reserveVB1),reserveNextNotify,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB1),reserveNextNotifyEdit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveBox),reserveTitle,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveBox),reserveVB2,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB2),reserveDescription,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB2),reserveHB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveCommunityList,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(reserveCommunityList),GTK_TREE_MODEL(reserveCommunityListStore));
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveVB3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB3),reserveCategoryList,FALSE,FALSE,0);
	gtk_combo_box_set_model(GTK_COMBO_BOX(reserveCategoryList),GTK_TREE_MODEL(reserveCategoryListStore));
	gtk_box_pack_start(GTK_BOX(reserveVB3),reserveFace,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB3),reserveCall,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB3),reserveCruise,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveVB4,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB4),reserveCommunityOnly,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB4),reserveTimeshift,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB4),reserveAdsense,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveVB4),reserveIchiba,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTwitterBox,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveTwitterBox),reserveTwitter,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveTwitterBox),reserveTwitterTags,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTB2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTB3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTB4,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveTB5,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB1),reserveTag0,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB1),reserveTag1,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB2),reserveTag2,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB2),reserveTag3,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB3),reserveTag4,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB3),reserveTag5,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB4),reserveTag6,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB4),reserveTag7,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB5),reserveTag8,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveTB5),reserveTag9,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(reserveHB1),reserveMusic,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(reserveMusic),reserveMusicLabel);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),commentSettingBox,commentSettingL);
	//gtk_container_add(GTK_CONTAINER(commentSettingEB),commentSettingL);
	gtk_box_pack_start(GTK_BOX(commentSettingBox),commentSettingHB1,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingHB1),commentSettingNGUsersBox,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingNGUsersBox),commentSettingNGUsersLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingNGUsersBox),commentSettingNGUsers,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(commentSettingNGUsers),GTK_TREE_MODEL(commentSettingNGUsersStore));
	gtk_box_pack_start(GTK_BOX(commentSettingNGUsersBox),commentSettingNGUsersDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingNGUsersDelete),commentSettingNGUsersDeleteLabel);
	gtk_box_pack_start(GTK_BOX(commentSettingNGUsersBox),commentSettingNGUsersAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingNGUsersAdd),commentSettingNGUsersAddLabel);
	gtk_box_pack_start(GTK_BOX(commentSettingHB1),commentSettingNGCommentsBox,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingNGCommentsBox),commentSettingNGCommentsLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingNGCommentsBox),commentSettingNGComments,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(commentSettingNGComments),GTK_TREE_MODEL(commentSettingNGCommentsStore));
	gtk_box_pack_start(GTK_BOX(commentSettingNGCommentsBox),commentSettingNGCommentsDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingNGCommentsDelete),commentSettingNGCommentsDeleteLabel);
	gtk_box_pack_start(GTK_BOX(commentSettingNGCommentsBox),commentSettingNGCommentsAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingNGCommentsAdd),commentSettingNGCommentsAddLabel);
	gtk_box_pack_start(GTK_BOX(commentSettingBox),commentSettingNGEdit,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingBox),commentSettingResponseListLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingBox),commentSettingResponseList,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(commentSettingResponseList),GTK_TREE_MODEL(commentSettingResponseListStore));
	gtk_box_pack_start(GTK_BOX(commentSettingBox),commentSettingHB5,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingHB5),commentSettingVB2,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingVB2),commentSettingQuestionBox,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingQuestionBox),commentSettingQuestionLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingQuestionBox),commentSettingQuestion,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingVB2),commentSettingResponseBox,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingResponseBox),commentSettingResponseLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingResponseBox),commentSettingResponse,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingHB5),commentSettingVB3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(commentSettingVB3),commentSettingResponseAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingResponseAdd),commentSettingResponseAddLabel);
	gtk_box_pack_start(GTK_BOX(commentSettingVB3),commentSettingResponseDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(commentSettingResponseDelete),commentSettingResponseDeleteLabel);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),handleNameBox,handleNameL);
	//gtk_container_add(GTK_CONTAINER(handleNameEB),handleNameL);
	gtk_box_pack_start(GTK_BOX(handleNameBox),handleNameHB1,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(handleNameHB1),handleNameRawB,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(handleNameRawB),handleNameRawLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleNameRawB),handleNameRawList,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(handleNameRawList),GTK_TREE_MODEL(handleNameRawListStore));
	gtk_box_pack_start(GTK_BOX(handleNameRawB),handleNameRawDelete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(handleNameRawDelete),handleNameRawDeleteLabel);
	gtk_box_pack_start(GTK_BOX(handleNameHB1),handleName184B,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(handleName184B),handleName184Label,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleName184B),handleName184Season,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleName184B),handleName184List,TRUE,TRUE,0);
	gtk_tree_view_set_model(GTK_TREE_VIEW(handleName184List),GTK_TREE_MODEL(handleName184ListStore));
	gtk_box_pack_start(GTK_BOX(handleName184B),handleName184Delete,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(handleName184Delete),handleName184DeleteLabel);
	gtk_box_pack_start(GTK_BOX(handleNameBox),handleNameVB2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleNameVB2),handleNameIDLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleNameVB2),handleNameID,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(handleNameVB2),handleNameHandleLabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(handleNameVB2),handleNameHandle,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(handleNameVB2),handleNameAdd,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(handleNameAdd),handleNameAddLabel);

	g_signal_connect(G_OBJECT(mainWindow),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_show_all(mainWindow);
	gtk_main();
}
