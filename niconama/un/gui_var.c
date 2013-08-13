#include <gtk/gtk.h>

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
	*alertIDEdit, *alertIDType, *alertReconnectLabel,
	*alertAdd, *alertDelete, *alertAddLabel,
	*alertReconnect, *alertStatus, *alertDeleteLabel,
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
	*reserveTagBox, *reserveTag0,
	*reserveTag1, *reserveTag2,
	*reserveTag3, *reserveTag4,
	*reserveTag5, *reserveTag6,
	*reserveTag7, *reserveTag8,
	*reserveTag9, *reserveMusic, *reserveMusicLabel,
	*commentSettingEB, *commentSettingL,
	*commentSettingBox, *commentSettingHB1,
	*commentSettingNGUsers,
	*commentSettingNGComments,
	*commentSettingHB2, *commentSettingNGEdit,
	*commentSettingVB1, *commentSettingHB3,
	*commentSettingNGUserAdd, *commentSettingNGUserAddLabel,
	*commentSettingNGUserDelete, *commentSettingNGUserDeleteLabel,
	*commentSettingHB4, *commentSettingNGCommentDeleteLabel,
	*commentSettingNGCommentAdd, *commentSettingNGCommentAddLabel,
	*commentSettingNGCommentDelete,
	*commentSettingResponseList,
	*commentSettingHB5, *commentSettingVB2,
	*commentSettingQuestion,
	*commentSettingResponse,
	*commentSettingVB3, *commentSettingResponseDeleteLabel,
	*commentSettingResponseAdd, *commentSettingResponseAddLabel,
	*commentSettingResponseDelete,
	*handleNameEB, *handleNameL,
	*handleNameBox, *handleNameHB1,
	*handleNameRawList, *handleNameVB1,
	*handleName184Season, *handleName184List,
	*handleNameHB2, *handleNameVB2,
	*handleNameID, *handleNameHandle,
	*handleNameVB3, *handleNameAdd, *handleNameDeleteLabel,
	*handleNameDelete, *handleNameAddLabel;
GtkListStore *commentConnectedListStore,
	*commentMainStore, *alertFavoritesStore,
	*alertIDTypeStore, *multimediaVideoSourceListStore,
	*multimediaPresetListStore, *reservePresetListStore,
	*reserveCommunityListStore, *reserveCategoryListStore,
	*commentSettingNGUsersStore, *commentSettingNGCommentsStore,
	*commentSettingResponseListStore,
	*handleNameRawListStore, *handleName184ListStore;
void create(){
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
	ffmpegStop=gtk_button_new();
	ffmpegStopLabel=gtk_label_new("停止");
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
	alertReconnect=gtk_button_new();
	alertReconnectLabel=gtk_label_new("再接続");
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
	multimediaBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	multimediaHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	multimediaPreview=gtk_image_new();
	multimediaVideoSourceList=gtk_tree_view_new();
	multimediaVideoSourceListStore=gtk_list_store_new(1,G_TYPE_STRING);//TODO
	multimediaVideoMenuBar=gtk_menu_bar_new();
	multimediaVideoMenuBarAdd=gtk_menu_item_new();
	multimediaVideoMenuBarAddLabel=gtk_label_new("追加");
	multimediaVideoMenuBarAddMenu=gtk_menu_new();


	multimediaVideoMenuBarDelete=gtk_menu_item_new();
	multimediaVideoMenuBarDeleteLabel=gtk_label_new("削除");
	multimediaVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	multimediaPresetList=gtk_combo_box_new();
	multimediaPresetListStore=gtk_list_store_new(1,G_TYPE_STRING);
	multimediaPresetName=gtk_entry_new();
	multimediaPresetAdd=gtk_button_new();
	multimediaPresetAddLabel=gtk_label_new("追加");
	multimediaPresetDelete=gtk_button_new();
	multimediaPresetDeleteLabel=gtk_label_new("削除");
	multimediaHB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	multimediaAudioMuxerBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);

	multimediaVB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	multimediaAudioAdd=gtk_button_new();
	multimediaAudioAddLabel=gtk_label_new("追加");
	multimediaAudioDelete=gtk_button_new();
	multimediaAudioDeleteLabel=gtk_label_new("削除");
	multimediaAdvancedSettings=gtk_frame_new(NULL);
	multimediaAdvancedSettingsLabel=gtk_label_new("拡張設定");
	reserveEB=gtk_event_box_new();
	reserveL=gtk_label_new("枠取り");
	reserveBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
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
	reserveVB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveDescription=gtk_text_view_new();
	reserveHB1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	reserveCommunityList=gtk_combo_box_new();
	reserveCommunityListStore=gtk_list_store_new(1,G_TYPE_STRING);
	reserveVB3=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveCategoryList=gtk_combo_box_new();
	reserveCategoryListStore=gtk_list_store_new(1,G_TYPE_STRING);
	reserveFace=gtk_check_button_new_with_label("顔出し");
	reserveCall=gtk_check_button_new_with_label("凸待ち");
	reserveCruise=gtk_check_button_new_with_label("クルーズ待ち");
	reserveVB4=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveCommunityOnly=gtk_check_button_new_with_label("コミュ限");
	reserveTimeshift=gtk_check_button_new_with_label("タイムシフト");
	reserveAdsense=gtk_check_button_new_with_label("広告");
	reserveIchiba=gtk_check_button_new_with_label("市場");
	reserveTwitterBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	reserveTwitter=gtk_check_button_new_with_label("Twitter");
	reserveTwitterTags=gtk_entry_new();
	reserveTagBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
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
	commentSettingNGUsers=gtk_tree_view_new();
	commentSettingNGUsersStore=gtk_list_store_new(1,G_TYPE_STRING);
	commentSettingNGComments=gtk_tree_view_new();
	commentSettingNGCommentsStore=gtk_list_store_new(1,G_TYPE_STRING);
	commentSettingHB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingNGEdit=gtk_entry_new();
	commentSettingVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingHB3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingNGUserAdd=gtk_button_new();
	commentSettingNGUserAddLabel=gtk_label_new("追加");
	commentSettingNGUserDelete=gtk_button_new();
	commentSettingNGUserDeleteLabel=gtk_label_new("削除");
	commentSettingHB4=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingNGCommentAdd=gtk_button_new();
	commentSettingNGCommentAddLabel=gtk_label_new("追加");
	commentSettingNGCommentDelete=gtk_button_new();
	commentSettingNGCommentDeleteLabel=gtk_label_new("削除");
	commentSettingResponseList=gtk_tree_view_new();
	commentSettingResponseListStore=gtk_list_store_new(1,G_TYPE_STRING);//TODO
	commentSettingHB5=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	commentSettingVB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	commentSettingQuestion=gtk_entry_new();
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
	handleNameRawList=gtk_tree_view_new();
	handleNameRawListStore=gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
	handleNameVB1=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleName184Season=gtk_label_new(NULL);
	handleName184List=gtk_tree_view_new();
	handleName184ListStore=gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
	handleNameHB2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	handleNameVB2=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleNameID=gtk_entry_new();
	handleNameHandle=gtk_entry_new();
	handleNameVB3=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	handleNameAdd=gtk_button_new();
	handleNameAddLabel=gtk_label_new("追加");
	handleNameDelete=gtk_button_new();
	handleNameDeleteLabel=gtk_label_new("削除");

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
	gtk_container_add(GTK_CONTAINER(mainToolbarPreset),multimediaPresetList);
	gtk_box_pack_start(GTK_BOX(mainBox),mainNotebook,TRUE,TRUE,0);

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),commentBox,commentEB);
	gtk_container_add(GTK_CONTAINER(commentEB),commentL);
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

	gtk_notebook_append_page(GTK_NOTEBOOK(mainNotebook),ffmpegBox,ffmpegEB);
	gtk_container_add(GTK_CONTAINER(ffmpegEB),ffmpegL);
	gtk_box_pack_start(GTK_BOX(ffmpegBox),ffmpegLog,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(ffmpegBox),ffmpegHB1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(ffmpegHB1),ffmpegStart,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(ffmpegStart),ffmpegStartLabel);
	gtk_box_pack_start(GTK_BOX(ffmpegHB1),ffmpegStop,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(ffmpegStop),ffmpegStopLabel);

	g_signal_connect(G_OBJECT(mainWindow),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_show_all(mainWindow);
	gtk_main();
}