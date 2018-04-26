import QtQuick 2.0

Rectangle {
    id: rootRect;

    property string imageBaseDir: "/media/datastore/ui/resources/CYC/assets/images/ambiance_1";

    x: 0;
    y: 0;
    width: parent == null ? 0 : parent.width;
    height: parent == null ? 0 : parent.height;

    color: "black";

    Flickable {
        anchors.fill: parent;
        contentWidth: width;
        contentHeight: 100000;

        MyImage {
            id: image_0;
        }

        MyImage {
            id: image_1;
            anchors.top: image_0.bottom;
            baseName: imageBaseDir + "/./btn_rightseat";
        }
        MyImage {
            id: image_2;
            anchors.top: image_1.bottom;
            baseName: imageBaseDir + "/./btn_src_usb_pic";
        }
        MyImage {
            id: image_3;
            anchors.top: image_2.bottom;
            baseName: imageBaseDir + "/./btn_phone_delete";
        }
        MyImage {
            id: image_4;
            anchors.top: image_3.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_6";
        }
        MyImage {
            id: image_5;
            anchors.top: image_4.bottom;
            baseName: imageBaseDir + "/./btn_phone_handfree";
        }
        MyImage {
            id: image_6;
            anchors.top: image_5.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_up";
        }
        MyImage {
            id: image_7;
            anchors.top: image_6.bottom;
            baseName: imageBaseDir + "/./btn_panelB_list1";
        }
        MyImage {
            id: image_8;
            anchors.top: image_7.bottom;
            baseName: imageBaseDir + "/./scn_setting_audio/btn_setting_audio";
        }
        MyImage {
            id: image_9;
            anchors.top: image_8.bottom;
            baseName: imageBaseDir + "/./btn_src_ipod";
        }
        MyImage {
            id: image_10;
            anchors.top: image_9.bottom;
            baseName: imageBaseDir + "/./btn_switch_off";
        }
        MyImage {
            id: image_11;
            anchors.top: image_10.bottom;
            baseName: imageBaseDir + "/./btn_phonepad_call";
        }
        MyImage {
            id: image_12;
            anchors.top: image_11.bottom;
            baseName: imageBaseDir + "/./btn_page_downlist";
        }
        MyImage {
            id: image_13;
            anchors.top: image_12.bottom;
            baseName: imageBaseDir + "/./btn_audio_setting";
        }
        MyImage {
            id: image_14;
            anchors.top: image_13.bottom;
            baseName: imageBaseDir + "/./btn_media_previous";
        }
        MyImage {
            id: image_15;
            anchors.top: image_14.bottom;
            baseName: imageBaseDir + "/./btn_reset";
        }
        MyImage {
            id: image_16;
            anchors.top: image_15.bottom;
            baseName: imageBaseDir + "/./btn_panelB_historylist";
        }
        MyImage {
            id: image_17;
            anchors.top: image_16.bottom;
            baseName: imageBaseDir + "/./btn_phone_contacts";
        }
        MyImage {
            id: image_18;
            anchors.top: image_17.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_1";
        }
        MyImage {
            id: image_19;
            anchors.top: image_18.bottom;
            baseName: imageBaseDir + "/./btn_media_sequence";
        }
        MyImage {
            id: image_20;
            anchors.top: image_19.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_last";
        }
        MyImage {
            id: image_21;
            anchors.top: image_20.bottom;
            baseName: imageBaseDir + "/./btn_src_sd_pic";
        }
        MyImage {
            id: image_22;
            anchors.top: image_21.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_previous";
        }
        MyImage {
            id: image_23;
            anchors.top: image_22.bottom;
            baseName: imageBaseDir + "/./btn_page_up";
        }
        MyImage {
            id: image_24;
            anchors.top: image_23.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_2";
        }
        MyImage {
            id: image_25;
            anchors.top: image_24.bottom;
            baseName: imageBaseDir + "/./btn_phone_dialpad";
        }
        MyImage {
            id: image_26;
            anchors.top: image_25.bottom;
            baseName: imageBaseDir + "/./btn_equalizer_equals";
        }
        MyImage {
            id: image_27;
            anchors.top: image_26.bottom;
            baseName: imageBaseDir + "/./btn_media_video_play";
        }
        MyImage {
            id: image_28;
            anchors.top: image_27.bottom;
            baseName: imageBaseDir + "/./btn_seek_up";
        }
        MyImage {
            id: image_29;
            anchors.top: image_28.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_shuffle";
        }
        MyImage {
            id: image_30;
            anchors.top: image_29.bottom;
            baseName: imageBaseDir + "/./btn_media_next";
        }
        MyImage {
            id: image_31;
            anchors.top: image_30.bottom;
            baseName: imageBaseDir + "/./btn_clear";
        }
        MyImage {
            id: image_32;
            anchors.top: image_31.bottom;
            baseName: imageBaseDir + "/./btn_navi_home";
        }
        MyImage {
            id: image_33;
            anchors.top: image_32.bottom;
            baseName: imageBaseDir + "/./btn_panelB_historylist_Arrow";
        }
        MyImage {
            id: image_34;
            anchors.top: image_33.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_reset";
        }
        MyImage {
            id: image_35;
            anchors.top: image_34.bottom;
            baseName: imageBaseDir + "/./btn_return";
        }
        MyImage {
            id: image_36;
            anchors.top: image_35.bottom;
            baseName: imageBaseDir + "/./btn_src_aux";
        }
        MyImage {
            id: image_37;
            anchors.top: image_36.bottom;
            baseName: imageBaseDir + "/./btn_media_setup";
        }
        MyImage {
            id: image_38;
            anchors.top: image_37.bottom;
            baseName: imageBaseDir + "/./btn_panelB_list3";
        }
        MyImage {
            id: image_39;
            anchors.top: image_38.bottom;
            baseName: imageBaseDir + "/./btn_media_video_down";
        }
        MyImage {
            id: image_40;
            anchors.top: image_39.bottom;
            baseName: imageBaseDir + "/./btn_navi_hangup";
        }
        MyImage {
            id: image_41;
            anchors.top: image_40.bottom;
            baseName: imageBaseDir + "/./scn_setup_time/btn_timebox_long";
        }
        MyImage {
            id: image_42;
            anchors.top: image_41.bottom;
            baseName: imageBaseDir + "/./btn_delete";
        }
        MyImage {
            id: image_43;
            anchors.top: image_42.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_repeatfolder";
        }
        MyImage {
            id: image_44;
            anchors.top: image_43.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_1";
        }
        MyImage {
            id: image_45;
            anchors.top: image_44.bottom;
            baseName: imageBaseDir + "/./btn_phone_keypad";
        }
        MyImage {
            id: image_46;
            anchors.top: image_45.bottom;
            baseName: imageBaseDir + "/./btn_page_uplist";
        }
        MyImage {
            id: image_47;
            anchors.top: image_46.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_shufflefolder";
        }
        MyImage {
            id: image_48;
            anchors.top: image_47.bottom;
            baseName: imageBaseDir + "/./btn_media_shuffle";
        }
        MyImage {
            id: image_49;
            anchors.top: image_48.bottom;
            baseName: imageBaseDir + "/./btn_space";
        }
        MyImage {
            id: image_50;
            anchors.top: image_49.bottom;
            baseName: imageBaseDir + "/./btn_navi_private";
        }
        MyImage {
            id: image_51;
            anchors.top: image_50.bottom;
            baseName: imageBaseDir + "/./btn_carplay";
        }
        MyImage {
            id: image_52;
            anchors.top: image_51.bottom;
            baseName: imageBaseDir + "/./btn_tune";
        }
        MyImage {
            id: image_53;
            anchors.top: image_52.bottom;
            baseName: imageBaseDir + "/./btn_operate_bg";
        }
        MyImage {
            id: image_54;
            anchors.top: image_53.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_player";
        }
        MyImage {
            id: image_55;
            anchors.top: image_54.bottom;
            baseName: imageBaseDir + "/./btn_src_sd_video";
        }
        MyImage {
            id: image_56;
            anchors.top: image_55.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_Wi-Fi";
        }
        MyImage {
            id: image_57;
            anchors.top: image_56.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_lights";
        }
        MyImage {
            id: image_58;
            anchors.top: image_57.bottom;
            baseName: imageBaseDir + "/./btn_equalizer_reset";
        }
        MyImage {
            id: image_59;
            anchors.top: image_58.bottom;
            baseName: imageBaseDir + "/./btn_src_bt";
        }
        MyImage {
            id: image_60;
            anchors.top: image_59.bottom;
            baseName: imageBaseDir + "/./btn_media_infoclose";
        }
        MyImage {
            id: image_61;
            anchors.top: image_60.bottom;
            baseName: imageBaseDir + "/./btn_src_page_up";
        }
        MyImage {
            id: image_62;
            anchors.top: image_61.bottom;
            baseName: imageBaseDir + "/./btn_listview_phone_detail";
        }
        MyImage {
            id: image_63;
            anchors.top: image_62.bottom;
            baseName: imageBaseDir + "/./btn_search_tuner";
        }
        MyImage {
            id: image_64;
            anchors.top: image_63.bottom;
            baseName: imageBaseDir + "/./btn_search";
        }
        MyImage {
            id: image_65;
            anchors.top: image_64.bottom;
            baseName: imageBaseDir + "/./btn_radio_list";
        }
        MyImage {
            id: image_66;
            anchors.top: image_65.bottom;
            baseName: imageBaseDir + "/./btn_media_pause";
        }
        MyImage {
            id: image_67;
            anchors.top: image_66.bottom;
            baseName: imageBaseDir + "/./btn_phone_hold";
        }
        MyImage {
            id: image_68;
            anchors.top: image_67.bottom;
            baseName: imageBaseDir + "/./btn_player";
        }
        MyImage {
            id: image_69;
            anchors.top: image_68.bottom;
            baseName: imageBaseDir + "/./btn_navi_accept";
        }
        MyImage {
            id: image_70;
            anchors.top: image_69.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_2";
        }
        MyImage {
            id: image_71;
            anchors.top: image_70.bottom;
            baseName: imageBaseDir + "/./btn_navi_handfree";
        }
        MyImage {
            id: image_72;
            anchors.top: image_71.bottom;
            baseName: imageBaseDir + "/./btn_tune_down";
        }
        MyImage {
            id: image_73;
            anchors.top: image_72.bottom;
            baseName: imageBaseDir + "/./btn_phone_mute";
        }
        MyImage {
            id: image_74;
            anchors.top: image_73.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_next";
        }
        MyImage {
            id: image_75;
            anchors.top: image_74.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_bluetooth";
        }
        MyImage {
            id: image_76;
            anchors.top: image_75.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_sequence";
        }
        MyImage {
            id: image_77;
            anchors.top: image_76.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_talking_book";
        }
        MyImage {
            id: image_78;
            anchors.top: image_77.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_artist";
        }
        MyImage {
            id: image_79;
            anchors.top: image_78.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_all_songs";
        }
        MyImage {
            id: image_80;
            anchors.top: image_79.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_audio";
        }
        MyImage {
            id: image_81;
            anchors.top: image_80.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_album";
        }
        MyImage {
            id: image_82;
            anchors.top: image_81.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_composer";
        }
        MyImage {
            id: image_83;
            anchors.top: image_82.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_genre";
        }
        MyImage {
            id: image_84;
            anchors.top: image_83.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_podcast";
        }
        MyImage {
            id: image_85;
            anchors.top: image_84.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_playlist";
        }
        MyImage {
            id: image_86;
            anchors.top: image_85.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_video";
        }
        MyImage {
            id: image_87;
            anchors.top: image_86.bottom;
            baseName: imageBaseDir + "/./scn_media_common/icon_photo";
        }
        MyImage {
            id: image_88;
            anchors.top: image_87.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_repeatall";
        }
        MyImage {
            id: image_89;
            anchors.top: image_88.bottom;
            baseName: imageBaseDir + "/./btn_home";
        }
        MyImage {
            id: image_90;
            anchors.top: image_89.bottom;
            baseName: imageBaseDir + "/./btn_camera_guideline_1";
        }
        MyImage {
            id: image_91;
            anchors.top: image_90.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_delete";
        }
        MyImage {
            id: image_92;
            anchors.top: image_91.bottom;
            baseName: imageBaseDir + "/./btn_phone_packup";
        }
        MyImage {
            id: image_93;
            anchors.top: image_92.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_shift_selected";
        }
        MyImage {
            id: image_94;
            anchors.top: image_93.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_5";
        }
        MyImage {
            id: image_95;
            anchors.top: image_94.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_time";
        }
        MyImage {
            id: image_96;
            anchors.top: image_95.bottom;
            baseName: imageBaseDir + "/./btn_radio_tunedown";
        }
        MyImage {
            id: image_97;
            anchors.top: image_96.bottom;
            baseName: imageBaseDir + "/./btn_phone_private";
        }
        MyImage {
            id: image_98;
            anchors.top: image_97.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_4";
        }
        MyImage {
            id: image_99;
            anchors.top: image_98.bottom;
            baseName: imageBaseDir + "/./btn_equalizer_add";
        }
        MyImage {
            id: image_100;
            anchors.top: image_99.bottom;
            baseName: imageBaseDir + "/./btn_tune_up";
        }
        MyImage {
            id: image_101;
            anchors.top: image_100.bottom;
            baseName: imageBaseDir + "/./btn_media_repeatall";
        }
        MyImage {
            id: image_102;
            anchors.top: image_101.bottom;
            baseName: imageBaseDir + "/./btn_inputpin_ok";
        }
        MyImage {
            id: image_103;
            anchors.top: image_102.bottom;
            baseName: imageBaseDir + "/./btn_navi_seekdown";
        }
        MyImage {
            id: image_104;
            anchors.top: image_103.bottom;
            baseName: imageBaseDir + "/./btn_media_play";
        }
        MyImage {
            id: image_105;
            anchors.top: image_104.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_defroster";
        }
        MyImage {
            id: image_106;
            anchors.top: image_105.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_off";
        }
        MyImage {
            id: image_107;
            anchors.top: image_106.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_fan_lower";
        }
        MyImage {
            id: image_108;
            anchors.top: image_107.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_fan_raise";
        }
        MyImage {
            id: image_109;
            anchors.top: image_108.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_fanup";
        }
        MyImage {
            id: image_110;
            anchors.top: image_109.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_mode_facefoot";
        }
        MyImage {
            id: image_111;
            anchors.top: image_110.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_auto";
        }
        MyImage {
            id: image_112;
            anchors.top: image_111.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_mode_foot";
        }
        MyImage {
            id: image_113;
            anchors.top: image_112.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_dual";
        }
        MyImage {
            id: image_114;
            anchors.top: image_113.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_circulation";
        }
        MyImage {
            id: image_115;
            anchors.top: image_114.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_mode_face";
        }
        MyImage {
            id: image_116;
            anchors.top: image_115.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_ac";
        }
        MyImage {
            id: image_117;
            anchors.top: image_116.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_fandown";
        }
        MyImage {
            id: image_118;
            anchors.top: image_117.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_tempdown";
        }
        MyImage {
            id: image_119;
            anchors.top: image_118.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_mode_footwindow";
        }
        MyImage {
            id: image_120;
            anchors.top: image_119.bottom;
            baseName: imageBaseDir + "/./ppp_hvac_setting/btn_hvac_tempup";
        }
        MyImage {
            id: image_121;
            anchors.top: image_120.bottom;
            baseName: imageBaseDir + "/./btn_switch_on";
        }
        MyImage {
            id: image_122;
            anchors.top: image_121.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_safety";
        }
        MyImage {
            id: image_123;
            anchors.top: image_122.bottom;
            baseName: imageBaseDir + "/./btn_radio_tuneup";
        }
        MyImage {
            id: image_124;
            anchors.top: image_123.bottom;
            baseName: imageBaseDir + "/./btn_phone_accept";
        }
        MyImage {
            id: image_125;
            anchors.top: image_124.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_pause";
        }
        MyImage {
            id: image_126;
            anchors.top: image_125.bottom;
            baseName: imageBaseDir + "/./btn_src_page_down";
        }
        MyImage {
            id: image_127;
            anchors.top: image_126.bottom;
            baseName: imageBaseDir + "/./btn_tuner_list";
        }
        MyImage {
            id: image_128;
            anchors.top: image_127.bottom;
            baseName: imageBaseDir + "/./btn_media_video_ff";
        }
        MyImage {
            id: image_129;
            anchors.top: image_128.bottom;
            baseName: imageBaseDir + "/./btn_src_usb_music";
        }
        MyImage {
            id: image_130;
            anchors.top: image_129.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_option";
        }
        MyImage {
            id: image_131;
            anchors.top: image_130.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_7";
        }
        MyImage {
            id: image_132;
            anchors.top: image_131.bottom;
            baseName: imageBaseDir + "/./btn_media_list";
        }
        MyImage {
            id: image_133;
            anchors.top: image_132.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_6";
        }
        MyImage {
            id: image_134;
            anchors.top: image_133.bottom;
            baseName: imageBaseDir + "/./btn_em_usb";
        }
        MyImage {
            id: image_135;
            anchors.top: image_134.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_status";
        }
        MyImage {
            id: image_136;
            anchors.top: image_135.bottom;
            baseName: imageBaseDir + "/./btn_refresh";
        }
        MyImage {
            id: image_137;
            anchors.top: image_136.bottom;
            baseName: imageBaseDir + "/./btn_src_usb_video";
        }
        MyImage {
            id: image_138;
            anchors.top: image_137.bottom;
            baseName: imageBaseDir + "/./btn_media_turnleft";
        }
        MyImage {
            id: image_139;
            anchors.top: image_138.bottom;
            baseName: imageBaseDir + "/./btn_connection";
        }
        MyImage {
            id: image_140;
            anchors.top: image_139.bottom;
            baseName: imageBaseDir + "/./btn_carlife";
        }
        MyImage {
            id: image_141;
            anchors.top: image_140.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_char";
        }
        MyImage {
            id: image_142;
            anchors.top: image_141.bottom;
            baseName: imageBaseDir + "/./btn_video_play";
        }
        MyImage {
            id: image_143;
            anchors.top: image_142.bottom;
            baseName: imageBaseDir + "/./btn_media_player";
        }
        MyImage {
            id: image_144;
            anchors.top: image_143.bottom;
            baseName: imageBaseDir + "/./btn_seek_down";
        }
        MyImage {
            id: image_145;
            anchors.top: image_144.bottom;
            baseName: imageBaseDir + "/./btn_camera_guideline_0";
        }
        MyImage {
            id: image_146;
            anchors.top: image_145.bottom;
            baseName: imageBaseDir + "/./btn_phone_resume";
        }
        MyImage {
            id: image_147;
            anchors.top: image_146.bottom;
            baseName: imageBaseDir + "/./btn_image_next";
        }
        MyImage {
            id: image_148;
            anchors.top: image_147.bottom;
            baseName: imageBaseDir + "/./btn_dts";
        }
        MyImage {
            id: image_149;
            anchors.top: image_148.bottom;
            baseName: imageBaseDir + "/./btn_media_video_up";
        }
        MyImage {
            id: image_150;
            anchors.top: image_149.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_down";
        }
        MyImage {
            id: image_151;
            anchors.top: image_150.bottom;
            baseName: imageBaseDir + "/./btn_media_repeatfolder";
        }
        MyImage {
            id: image_152;
            anchors.top: image_151.bottom;
            baseName: imageBaseDir + "/./btn_image_last";
        }
        MyImage {
            id: image_153;
            anchors.top: image_152.bottom;
            baseName: imageBaseDir + "/./btn_input_method";
        }
        MyImage {
            id: image_154;
            anchors.top: image_153.bottom;
            baseName: imageBaseDir + "/./btn_media_turnright";
        }
        MyImage {
            id: image_155;
            anchors.top: image_154.bottom;
            baseName: imageBaseDir + "/./btn_navi_popup";
        }
        MyImage {
            id: image_156;
            anchors.top: image_155.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_seat";
        }
        MyImage {
            id: image_157;
            anchors.top: image_156.bottom;
            baseName: imageBaseDir + "/./btn_navi_seekup";
        }
        MyImage {
            id: image_158;
            anchors.top: image_157.bottom;
            baseName: imageBaseDir + "/./btn_page_down";
        }
        MyImage {
            id: image_159;
            anchors.top: image_158.bottom;
            baseName: imageBaseDir + "/./btn_src_sd_music";
        }
        MyImage {
            id: image_160;
            anchors.top: image_159.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_singlecycle";
        }
        MyImage {
            id: image_161;
            anchors.top: image_160.bottom;
            baseName: imageBaseDir + "/./btn_media_songlist";
        }
        MyImage {
            id: image_162;
            anchors.top: image_161.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_space";
        }
        MyImage {
            id: image_163;
            anchors.top: image_162.bottom;
            baseName: imageBaseDir + "/./btn_media_repeat1";
        }
        MyImage {
            id: image_164;
            anchors.top: image_163.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_audio";
        }
        MyImage {
            id: image_165;
            anchors.top: image_164.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_arrow_left";
        }
        MyImage {
            id: image_166;
            anchors.top: image_165.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_radio";
        }
        MyImage {
            id: image_167;
            anchors.top: image_166.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_freelink";
        }
        MyImage {
            id: image_168;
            anchors.top: image_167.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_vehicle";
        }
        MyImage {
            id: image_169;
            anchors.top: image_168.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_carplay";
        }
        MyImage {
            id: image_170;
            anchors.top: image_169.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_arrow_right";
        }
        MyImage {
            id: image_171;
            anchors.top: image_170.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_media";
        }
        MyImage {
            id: image_172;
            anchors.top: image_171.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_bt";
        }
        MyImage {
            id: image_173;
            anchors.top: image_172.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_carlife";
        }
        MyImage {
            id: image_174;
            anchors.top: image_173.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_setting";
        }
        MyImage {
            id: image_175;
            anchors.top: image_174.bottom;
            baseName: imageBaseDir + "/./scn_home_main/btn_home_navigation";
        }
        MyImage {
            id: image_176;
            anchors.top: image_175.bottom;
            baseName: imageBaseDir + "/./btn_arrow_right";
        }
        MyImage {
            id: image_177;
            anchors.top: image_176.bottom;
            baseName: imageBaseDir + "/./btn_navi_media_next";
        }
        MyImage {
            id: image_178;
            anchors.top: image_177.bottom;
            baseName: imageBaseDir + "/./btn_radio_fm";
        }
        MyImage {
            id: image_179;
            anchors.top: image_178.bottom;
            baseName: imageBaseDir + "/./scn_em_common/nextinterface_arrow";
        }
        MyImage {
            id: image_180;
            anchors.top: image_179.bottom;
            baseName: imageBaseDir + "/./btn_keyboard_shift";
        }
        MyImage {
            id: image_181;
            anchors.top: image_180.bottom;
            baseName: imageBaseDir + "/./btn_media_video_rew";
        }
        MyImage {
            id: image_182;
            anchors.top: image_181.bottom;
            baseName: imageBaseDir + "/./btn_phone_unmute";
        }
        MyImage {
            id: image_183;
            anchors.top: image_182.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_4";
        }
        MyImage {
            id: image_184;
            anchors.top: image_183.bottom;
            baseName: imageBaseDir + "/./btn_media_phone";
        }
        MyImage {
            id: image_185;
            anchors.top: image_184.bottom;
            baseName: imageBaseDir + "/./btn_leftseat";
        }
        MyImage {
            id: image_186;
            anchors.top: image_185.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_5";
        }
        MyImage {
            id: image_187;
            anchors.top: image_186.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_comfort";
        }
        MyImage {
            id: image_188;
            anchors.top: image_187.bottom;
            baseName: imageBaseDir + "/./btn_arrow_up";
        }
        MyImage {
            id: image_189;
            anchors.top: image_188.bottom;
            baseName: imageBaseDir + "/./btn_popup";
        }
        MyImage {
            id: image_190;
            anchors.top: image_189.bottom;
            baseName: imageBaseDir + "/./btn_phone_holdcontact";
        }
        MyImage {
            id: image_191;
            anchors.top: image_190.bottom;
            baseName: imageBaseDir + "/./btn_media_fit";
        }
        MyImage {
            id: image_192;
            anchors.top: image_191.bottom;
            baseName: imageBaseDir + "/./btn_navi_src";
        }
        MyImage {
            id: image_193;
            anchors.top: image_192.bottom;
            baseName: imageBaseDir + "/./btn_volume";
        }
        MyImage {
            id: image_194;
            anchors.top: image_193.bottom;
            baseName: imageBaseDir + "/./btn_camera_guideline_2";
        }
        MyImage {
            id: image_195;
            anchors.top: image_194.bottom;
            baseName: imageBaseDir + "/./btn_settingmain_general";
        }
        MyImage {
            id: image_196;
            anchors.top: image_195.bottom;
            baseName: imageBaseDir + "/./btn_media_video_list";
        }
        MyImage {
            id: image_197;
            anchors.top: image_196.bottom;
            baseName: imageBaseDir + "/./btn_camera_guideline_3";
        }
        MyImage {
            id: image_198;
            anchors.top: image_197.bottom;
            baseName: imageBaseDir + "/./btn_vehiclemain_HVAC";
        }
        MyImage {
            id: image_199;
            anchors.top: image_198.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist1_3";
        }
        MyImage {
            id: image_200;
            anchors.top: image_199.bottom;
            baseName: imageBaseDir + "/./btn_media_video_pause";
        }
        MyImage {
            id: image_201;
            anchors.top: image_200.bottom;
            baseName: imageBaseDir + "/./scn_phone_main/icon_phone_audio";
        }
        MyImage {
            id: image_202;
            anchors.top: image_201.bottom;
            baseName: imageBaseDir + "/./scn_phone_main/icon_phone_history";
        }
        MyImage {
            id: image_203;
            anchors.top: image_202.bottom;
            baseName: imageBaseDir + "/./scn_phone_main/icon_phone_dialpad";
        }
        MyImage {
            id: image_204;
            anchors.top: image_203.bottom;
            baseName: imageBaseDir + "/./scn_phone_main/icon_phone_contacts";
        }
        MyImage {
            id: image_205;
            anchors.top: image_204.bottom;
            baseName: imageBaseDir + "/./scn_phone_main/icon_phone_connection";
        }
        MyImage {
            id: image_206;
            anchors.top: image_205.bottom;
            baseName: imageBaseDir + "/./btn_phone_hangup";
        }
        MyImage {
            id: image_207;
            anchors.top: image_206.bottom;
            baseName: imageBaseDir + "/./btn_phone_swap";
        }
        MyImage {
            id: image_208;
            anchors.top: image_207.bottom;
            baseName: imageBaseDir + "/./btn_arrow_down";
        }
        MyImage {
            id: image_209;
            anchors.top: image_208.bottom;
            baseName: imageBaseDir + "/./btn_panelAlist2_3";
        }
        MyImage {
            id: image_210;
            anchors.top: image_209.bottom;
            baseName: imageBaseDir + "/./btn_arrow_left";
        }

    }
}
