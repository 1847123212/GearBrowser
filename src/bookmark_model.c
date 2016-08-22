#include <dlog.h>
#include <eina_types.h>
#include <stdlib.h>
#include "gearbrowser.h"
#include "bookmark_model.h"
#include "settings.h"

Eina_Bool
bookmark_model_add(const char *title, const char *url) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_add] %s, %s", title, url);

	GError *error = NULL;
	JsonParser *parser = json_parser_new();
	const char *json = settings_value_get_n(PREF_KEY_BOOKMARKS);
	json_parser_load_from_data(parser, json, strlen(json), &error);
	if (error) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[bookmark_model_add] %s", error);
		g_error_free(error);
	} else {
		JsonObject *obj = json_object_new();
		json_object_set_string_member(obj, "title", title);
		json_object_set_string_member(obj, "url", url);

		JsonNode *root = json_parser_get_root(parser);
		JsonArray *items = json_node_get_array(root);
		json_array_add_object_element(items, obj);

		JsonGenerator *generator = json_generator_new();
		json_generator_set_root(generator, root);
		gchar *result = json_generator_to_data(generator, NULL);
		dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_add] json:%s", result);
		int ret = settings_value_set(PREF_KEY_BOOKMARKS, result);
		if (ret == 0) return EINA_TRUE;
		else dlog_print(DLOG_ERROR, LOG_TAG, "[bookmark_model_add] error_code:%d", ret);

		json_node_free(root);
		g_object_unref(generator);
		generator = NULL;
	}

	free((void *)json);
	g_object_unref(parser);
	json = NULL;
	parser = NULL;

	return EINA_FALSE;
}

Eina_Bool
bookmark_model_remove(BookmarkModel *item) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_remove] %s, %s", item->title, item->url);

	GError *error = NULL;
	JsonParser *parser = json_parser_new();
	const char *json = settings_value_get_n(PREF_KEY_BOOKMARKS);
	json_parser_load_from_data(parser, json, strlen(json), &error);
	if (error) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[bookmark_model_remove] %s", error);
		g_error_free(error);
	} else {
		JsonNode *root = json_parser_get_root(parser);
		JsonArray *items = json_node_get_array(root);
		guint size = json_array_get_length(items);
		for (int i = 0; i < size; ++i) {
			JsonObject *obj = json_array_get_object_element(items, i);
			if (!strcmp(item->title, json_object_get_string_member(obj, "title")) &&
					!strcmp(item->url, json_object_get_string_member(obj, "url"))) {
				dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_remove] [%d] found", i);
				json_array_remove_element(items, i);
				break;
			}
		}

		JsonGenerator *generator = json_generator_new();
		json_generator_set_root(generator, root);
		gchar *result = json_generator_to_data(generator, NULL);
		dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_remove] json:%s", result);
		int ret = settings_value_set(PREF_KEY_BOOKMARKS, result);
		if (ret == 0) return EINA_TRUE;
		else dlog_print(DLOG_ERROR, LOG_TAG, "[bookmark_model_remove] error_code:%d", ret);

		json_node_free(root);
		g_object_unref(generator);
		generator = NULL;
	}

	free((void *)json);
	g_object_unref(parser);
	json = NULL;
	parser = NULL;

	return EINA_FALSE;
}

GPtrArray*
bookmark_model_get_list_n(void) {
	const char *json = settings_value_get_n(PREF_KEY_BOOKMARKS);
	dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_get_list_n] %s", json);

	GPtrArray *items = g_ptr_array_new();
	GError *error = NULL;
	JsonParser *parser = json_parser_new();
	json_parser_load_from_data(parser, json, strlen(json), &error);
	if (error) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[bookmark_model_get_list_n] %s", error);
		g_error_free(error);
	} else {
		JsonNode *root = json_parser_get_root(parser);
		JsonArray *arr = json_node_get_array(root);
		int size = json_array_get_length(arr);
		dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_get_list_n] size:%d", size);
		for (int i = 0; i < size; ++i) {
			JsonObject *obj = json_array_get_object_element(arr, i);
			BookmarkModel *item = (BookmarkModel *)malloc(sizeof(BookmarkModel));
			item->title = strdup(json_object_get_string_member(obj, "title"));
			item->url = strdup(json_object_get_string_member(obj, "url"));
			g_ptr_array_add(items, item);
			dlog_print(DLOG_DEBUG, LOG_TAG, "[bookmark_model_get_list_n] %d, %s, %s", i, item->title, item->url);
		}

		json_node_free(root);
	}

	free((void *)json);
	g_object_unref(parser);
	json = NULL;
	parser = NULL;

	return items;
}
