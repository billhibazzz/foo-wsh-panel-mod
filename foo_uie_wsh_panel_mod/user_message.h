#pragma once

enum t_user_message
{
	UWM_SCRIPT_ERROR = WM_APP + 100,
	UWM_SCRIPT_ERROR_TIMEOUT,
	UWM_SCRIPT_DISABLE,
	UWM_TIMER,
	UWM_SIZE,
	UWM_REFRESHBK,
	UWM_SHOWCONFIGURE,
	UWM_SHOWPROPERTIES,
	UWM_GETALBUMARTASYNCDONE,
	UWM_REFRESHBKDONE,
	UWM_TOGGLEQUERYCONTINUE,
	UWM_GETCONFIGGUID,
	// config_object_notify
	UWM_PLAYLIST_STOP_AFTER_CURRENT,
	UWM_CURSOR_FOLLOW_PLAYBACK,
	UWM_PLAYBACK_FOLLOW_CURSOR,
	UWM_NOTIFY_DATA,
	// playback_statistics_collector
	UWM_ON_ITEM_PLAYED,
};
