#include "brutal_htt_parse.h"

VALUE rb_mLlhttpRb;
VALUE rb_cParser;

typedef struct {
  llhttp_t *parser;
  llhttp_settings_t *settings;
} llhttp_parser_t;

static void free_llhttp(void *_this) {
  llhttp_parser_t *parser = (llhttp_parser_t *)_this;
  if (parser == NULL) return;
  free(parser->parser);
  free(parser->settings);
}

static size_t llhttp_memsize(const void *_this) {
  const llhttp_parser_t *this = (const llhttp_parser_t *)_this;

  if (this == NULL) return 0;

  return sizeof(*this);
}

static const rb_data_type_t llhttp_parser_type = {
    "llhttp",
    {0, free_llhttp, llhttp_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

FOR_EACH_CALLBACK_METHOD(DEFINE_LLHTTP_TO_RUBY_CALLBACK);
FOR_EACH_DATA_CALLBACK_METHOD(DEFINE_LLHTTP_TO_RUBY_DATA_CALLBACK);
FOR_EACH_CALLBACK_METHOD(DEFINE_LLHTTP_CALLBACK);
FOR_EACH_DATA_CALLBACK_METHOD(DEFINE_LLHTTP_DATA_CALLBACK);

static VALUE parser_alloc(VALUE klass) {
  llhttp_parser_t *parser;
  VALUE rb_parser = TypedData_Make_Struct(klass, llhttp_parser_t, &llhttp_parser_type, parser);

  parser->settings = malloc(sizeof(llhttp_settings_t));
  parser->parser = malloc(sizeof(llhttp_t));
  llhttp_settings_init(parser->settings);
  FOR_EACH_CALLBACKS(DEFINE_CALLBACK_ASSIGNMENT);

  llhttp_init(parser->parser, HTTP_REQUEST, parser->settings);
  parser->parser->data = (void *)rb_parser;
  return rb_parser;
}


static void *gvl_llhttp_execute(void *data) {
  gvl_llhttp_execute_params_t *params = (gvl_llhttp_execute_params_t *)data;
  params->error = llhttp_execute(params->parser, params->request, params->request_len);
  return &params->error;
}

static VALUE execute(VALUE self, VALUE request) {
  llhttp_parser_t *parser;
  TypedData_Get_Struct(self, llhttp_parser_t, &llhttp_parser_type, parser);

  VALUE request_to_s = StringValue(request);

  gvl_llhttp_execute_params_t llhttp_execute_params = {
    .parser = parser->parser,
    .request = RSTRING_PTR(request_to_s),
    .request_len = RSTRING_LEN(request_to_s)
  };
  llhttp_errno_t *error = (llhttp_errno_t *)rb_thread_call_without_gvl(gvl_llhttp_execute, &llhttp_execute_params, RUBY_UBF_PROCESS, NULL);

  if (error != HPE_OK) {
    return Qtrue;
  } else {
    printf("parsed FAILED\n");
  }

  return UINT2NUM(0);
}

FOR_EACH_CALLBACK_METHOD(DEFINE_CALLBACK_METHOD);
FOR_EACH_DATA_CALLBACK_METHOD(DEFINE_DATA_CALLBACK_METHOD);

RUBY_FUNC_EXPORTED void
Init_brutal_htt_parse(void)
{
  rb_mLlhttpRb = rb_define_module("BrutalHTTParse");
  rb_cParser = rb_define_class_under(rb_mLlhttpRb, "Parser", rb_cObject);
  rb_define_alloc_func(rb_cParser, parser_alloc);
  rb_define_method(rb_cParser, "execute", execute, 1);
  FOR_EACH_CALLBACK_METHOD(DEFINE_RUBY_CALLBACK_METHOD);
  FOR_EACH_DATA_CALLBACK_METHOD(DEFINE_RUBY_DATA_CALLBACK_METHOD);
}
