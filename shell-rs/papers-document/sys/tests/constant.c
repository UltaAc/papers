// Generated by gir (https://github.com/gtk-rs/gir @ eb5be4f1bafe)
// from ../../ev-girs (@ 42c6b6028509+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#include "manual.h"
#include <stdio.h>

#define PRINT_CONSTANT(CONSTANT_NAME) \
    printf("%s;", #CONSTANT_NAME); \
    printf(_Generic((CONSTANT_NAME), \
                    char *: "%s", \
                    const char *: "%s", \
                    char: "%c", \
                    signed char: "%hhd", \
                    unsigned char: "%hhu", \
                    short int: "%hd", \
                    unsigned short int: "%hu", \
                    int: "%d", \
                    unsigned int: "%u", \
                    long: "%ld", \
                    unsigned long: "%lu", \
                    long long: "%lld", \
                    unsigned long long: "%llu", \
                    float: "%f", \
                    double: "%f", \
                    long double: "%ld"), \
           CONSTANT_NAME); \
    printf("\n");

int main() {
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_ALL);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_AREA);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_ATTACHMENT);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_COLOR);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_CONTENTS);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_LABEL);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_NONE);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_OPACITY);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_POPUP_IS_OPEN);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_POPUP_RECT);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_TEXT_ICON);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_TEXT_IS_OPEN);
    PRINT_CONSTANT((guint) PPS_ANNOTATIONS_SAVE_TEXT_MARKUP_TYPE);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_OVER_MARKUP_NOT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_OVER_MARKUP_NOT_IMPLEMENTED);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_OVER_MARKUP_UNKNOWN);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_OVER_MARKUP_YES);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_CIRCLE);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_COMMENT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_CROSS);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_HELP);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_INSERT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_KEY);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_NEW_PARAGRAPH);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_NOTE);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_PARAGRAPH);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_ICON_UNKNOWN);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_MARKUP_SQUIGGLY);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_MARKUP_STRIKE_OUT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TEXT_MARKUP_UNDERLINE);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TYPE_ATTACHMENT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TYPE_TEXT);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TYPE_TEXT_MARKUP);
    PRINT_CONSTANT((gint) PPS_ANNOTATION_TYPE_UNKNOWN);
    PRINT_CONSTANT((gint) PPS_COMPRESSION_BZIP2);
    PRINT_CONSTANT((gint) PPS_COMPRESSION_GZIP);
    PRINT_CONSTANT((gint) PPS_COMPRESSION_LZMA);
    PRINT_CONSTANT((gint) PPS_COMPRESSION_NONE);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_CONTAINS_JS_NO);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_CONTAINS_JS_UNKNOWN);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_CONTAINS_JS_YES);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_ERROR_ENCRYPTED);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_ERROR_INVALID);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_ERROR_UNSUPPORTED_CONTENT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_AUTHOR);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_CONTAINS_JS);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_CREATION_DATETIME);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_CREATOR);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_FORMAT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_KEYWORDS);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_LAYOUT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_LICENSE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_LINEARIZED);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_MOD_DATETIME);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_N_PAGES);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_PAPER_SIZE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_PERMISSIONS);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_PRODUCER);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_SECURITY);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_START_MODE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_SUBJECT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_TITLE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_INFO_UI_HINTS);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_ONE_COLUMN);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_SINGLE_PAGE);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_TWO_COLUMN_LEFT);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_TWO_COLUMN_RIGHT);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_TWO_PAGE_LEFT);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_LAYOUT_TWO_PAGE_RIGHT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_LOAD_FLAG_NONE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_LOAD_FLAG_NO_CACHE);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_FULL_SCREEN);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_NONE);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_PRESENTATION);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_USE_ATTACHMENTS);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_USE_OC);
    PRINT_CONSTANT((gint) PPS_DOCUMENT_MODE_USE_THUMBS);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_PERMISSIONS_FULL);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_ADD_NOTES);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_COPY);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_MODIFY);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_PRINT);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_CENTER_WINDOW);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_DIRECTION_RTL);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_DISPLAY_DOC_TITLE);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_FIT_WINDOW);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_HIDE_MENUBAR);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_HIDE_TOOLBAR);
    PRINT_CONSTANT((guint) PPS_DOCUMENT_UI_HINT_HIDE_WINDOWUI);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_COLLATE);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_COPIES);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_GENERATE_PDF);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_GENERATE_PS);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_NUMBER_UP);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_PAGE_SET);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_PREVIEW);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_REVERSE);
    PRINT_CONSTANT((guint) PPS_FILE_EXPORTER_CAN_SCALE);
    PRINT_CONSTANT((gint) PPS_FILE_FORMAT_PDF);
    PRINT_CONSTANT((gint) PPS_FILE_FORMAT_PS);
    PRINT_CONSTANT((gint) PPS_FILE_FORMAT_UNKNOWN);
    PRINT_CONSTANT((guint) PPS_FIND_CASE_SENSITIVE);
    PRINT_CONSTANT((guint) PPS_FIND_DEFAULT);
    PRINT_CONSTANT((guint) PPS_FIND_WHOLE_WORDS_ONLY);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_BUTTON_CHECK);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_BUTTON_PUSH);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_BUTTON_RADIO);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_CHOICE_COMBO);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_CHOICE_LIST);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_TEXT_FILE_SELECT);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_TEXT_MULTILINE);
    PRINT_CONSTANT((gint) PPS_FORM_FIELD_TEXT_NORMAL);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_EXTERNAL_URI);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_GOTO_DEST);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_GOTO_REMOTE);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_LAUNCH);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_LAYERS_STATE);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_NAMED);
    PRINT_CONSTANT((gint) PPS_LINK_ACTION_TYPE_RESET_FORM);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_FIT);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_FITH);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_FITR);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_FITV);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_NAMED);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_PAGE);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_PAGE_LABEL);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_UNKNOWN);
    PRINT_CONSTANT((gint) PPS_LINK_DEST_TYPE_XYZ);
    PRINT_CONSTANT(PPS_MAJOR_VERSION);
    PRINT_CONSTANT((gint) PPS_SELECTION_STYLE_GLYPH);
    PRINT_CONSTANT((gint) PPS_SELECTION_STYLE_LINE);
    PRINT_CONSTANT((gint) PPS_SELECTION_STYLE_WORD);
    PRINT_CONSTANT((gint) PPS_TRANSITION_ALIGNMENT_HORIZONTAL);
    PRINT_CONSTANT((gint) PPS_TRANSITION_ALIGNMENT_VERTICAL);
    PRINT_CONSTANT((gint) PPS_TRANSITION_DIRECTION_INWARD);
    PRINT_CONSTANT((gint) PPS_TRANSITION_DIRECTION_OUTWARD);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_BLINDS);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_BOX);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_COVER);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_DISSOLVE);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_FADE);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_FLY);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_GLITTER);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_PUSH);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_REPLACE);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_SPLIT);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_UNCOVER);
    PRINT_CONSTANT((gint) PPS_TRANSITION_EFFECT_WIPE);
    return 0;
}
