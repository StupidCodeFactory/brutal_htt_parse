#ifndef BRUTAL_HTT_PARSE_H
#define BRUTAL_HTT_PARSE_H 1

#include "llhttp.h"
#include <ruby.h>
#include <ruby/thread.h>
#include <stdio.h>

#define DEFINE_CALLBACK_METHOD(name) \
  static VALUE rb_##name(VALUE self) { return Qnil; }

#define DEFINE_DATA_CALLBACK_METHOD(name)               \
  static VALUE rb_##name(VALUE self, VALUE data) { return Qnil; }

#define DEFINE_RUBY_CALLBACK_METHOD(name)       \
  rb_define_method(rb_cParser, #name, rb_##name, 0);

#define DEFINE_RUBY_DATA_CALLBACK_METHOD(name)                                      \
  rb_define_method(rb_cParser, #name, rb_##name, 1);

#define DEFINE_CALLBACK_ASSIGNMENT(name) \
  parser->settings->name = name;

#define DEFINE_LLHTTP_TO_RUBY_CALLBACK(name)                            \
  static void *name##_cb(void *data) {                                  \
    rb_funcall((VALUE)data, rb_intern(#name), 0);                       \
    return NULL;                                                        \
  }

#define DEFINE_LLHTTP_TO_RUBY_DATA_CALLBACK(name)                       \
  static void *name##_cb(void *data) {                                  \
    callback_data_params_t *params = (callback_data_params_t *) data;   \
    VALUE parsed_data = rb_str_new(params->data, params->length);       \
    rb_funcall((VALUE)params->parser, rb_intern(#name), 1, parsed_data); \
    return NULL;                                                        \
  }

#define DEFINE_LLHTTP_CALLBACK(name)                                    \
  static int name(llhttp_t * parser) {                                  \
    VALUE ruby_parser = (VALUE)parser->data;                            \
    rb_thread_call_with_gvl(name##_cb, (void *)ruby_parser);            \
    return 0;                                                           \
  }

#define DEFINE_LLHTTP_DATA_CALLBACK(name)                               \
  static int name(llhttp_t* parser, const char *at, size_t length) {    \
    VALUE ruby_parser = (VALUE)parser->data;                            \
    callback_data_params_t params = {                                   \
      .parser = ruby_parser,                                            \
      .data = at,                                                       \
      .length = length                                                  \
    };                                                                  \
    rb_thread_call_with_gvl(name##_cb, (void *)&params);                \
    return 0;                                                           \
  }

#define FOR_EACH_DATA_CALLBACK_METHOD(X) \
  X(on_url)                              \
  X(on_status)                      \
  X(on_method)                      \
  X(on_version)                        \
  X(on_header_field)                   \
  X(on_header_value)                   \
  X(on_chunk_extension_name)           \
  X(on_chunk_extension_value)          \
  X(on_body)

#define FOR_EACH_CALLBACK_METHOD(X)                \
  X(on_message_begin)                              \
  X(on_headers_complete)                         \
  X(on_message_complete)                         \
  X(on_url_complete)                             \
  X(on_status_complete)                          \
  X(on_method_complete)                          \
  X(on_version_complete)                         \
  X(on_header_field_complete)                    \
  X(on_header_value_complete)                    \
  X(on_chunk_extension_name_complete)             \
  X(on_chunk_extension_value_complete)            \
  X(on_chunk_header)                             \
  X(on_chunk_complete)                           \
  X(on_reset)

#define FOR_EACH_CALLBACKS(X)       \
  FOR_EACH_CALLBACK_METHOD(X)         \
  FOR_EACH_DATA_CALLBACK_METHOD(X)    \



typedef struct gvl_llhttp_execute_params {
  llhttp_t *parser;
  char *request;
  size_t request_len;
  llhttp_errno_t error;
} gvl_llhttp_execute_params_t;


typedef struct callback_data_params {
  VALUE parser;
  const char *data;
  size_t length;
} callback_data_params_t;

#endif /* BRUTAL_HTT_PARSE_H */
