#include "xslt_ex.h"

static unsigned char *
binary_to_string(ErlNifBinary *bin)
{
  unsigned char *str = (unsigned char *)enif_alloc(bin->size + 1);

  if (!str)
  {
    enif_free(str);
    return NULL;
  }

  strncpy((char *)str, (char *)bin->data, bin->size);
  str[bin->size] = '\0';

  return str;
}

static char **
parse_xslt_params(ErlNifEnv *env, ERL_NIF_TERM input_params)
{
  ERL_NIF_TERM xslt_params_left = input_params;
  ERL_NIF_TERM xslt_param;
  int xslt_param_position = 0;
  unsigned int num_xslt_params;
  char **result_params;

  // Get ammount of params
  enif_get_list_length(env, input_params, &num_xslt_params);

  // Allocate space
  result_params = (char **)enif_alloc(sizeof(char *) * (num_xslt_params * 2 + 1));

  // Process each tuple of the parameters list
  while (enif_get_list_cell(env, xslt_params_left, &xslt_param, &xslt_params_left))
  {
    const ERL_NIF_TERM *tuple;
    ErlNifBinary key;
    ErlNifBinary value;
    const xmlChar *value_separator;
    xmlChar *value_clear;
    int num_elements;

    // Get tuple and check that it's as expected
    if (!enif_get_tuple(env, xslt_param, &num_elements, &tuple))
      return NULL;

    if (num_elements != 2 || !enif_inspect_binary(env, tuple[0], &key) || !enif_inspect_binary(env, tuple[1], &value))
      return NULL;

    // Extract binary key
    char *key_str = (char *)enif_alloc(sizeof(char) * key.size);
    strncpy((char *)key_str, (char *)key.data, key.size);
    key_str[key.size] = '\0';

    // Extract binary value
    char *value_str = (char *)enif_alloc(sizeof(char) * value.size);
    strncpy((char *)value_str, (char *)value.data, value.size);
    value_str[value.size] = '\0';

    // Encapsulate value as expected to be recognised
    if (xmlStrchr((const xmlChar *)value_str, '"'))
    {
      if (xmlStrchr((const xmlChar *)value_str, '\''))
      {
        return NULL;
      }
      value_separator = (const xmlChar *)"'";
    }
    else
    {
      value_separator = (const xmlChar *)"\"";
    }

    value_clear = xmlStrdup(value_separator);
    value_clear = xmlStrcat(value_clear, (const xmlChar *)value_str);
    value_clear = xmlStrcat(value_clear, value_separator);

    // ... and add it to the result
    result_params[xslt_param_position++] = key_str;
    result_params[xslt_param_position++] = (char *)value_clear;
  }

  result_params[xslt_param_position] = NULL;

  return result_params;
}

static ERL_NIF_TERM
transform(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  ERL_NIF_TERM term;
  ErlNifBinary arg0, arg1;

  const char **xslt_params;

  unsigned char *xslt_filename;
  xsltStylesheetPtr xslt;

  unsigned char *input_xml_string;
  xmlDocPtr input_xml, xslt_result;
  xmlChar *doc_txt_ptr;
  int doc_txt_len;

  if (argc < 2 || !enif_inspect_binary(env, argv[0], &arg0) || !enif_inspect_binary(env, argv[1], &arg1) || !enif_is_list(env, argv[2]))
    return enif_make_badarg(env);

  // Parse XSLT filename and input data
  xslt_filename = binary_to_string(&arg0);
  input_xml_string = binary_to_string(&arg1);

  if (!xslt_filename || !input_xml_string)
    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "enomem"));

  // Parse params
  xslt_params = (const char **)parse_xslt_params(env, argv[2]);

  if (!xslt_params)
  {
    enif_free(xslt_filename);
    enif_free(input_xml_string);

    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "param_not_valid"));
  }

  xslt = xsltParseStylesheetFile(xslt_filename);
  enif_free(xslt_filename);

  if (!xslt)
  {
    enif_free(input_xml_string);
    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "no_stylesheet"));
  }

  input_xml = xmlParseDoc((const xmlChar *)input_xml_string);
  enif_free(input_xml_string);

  if (!input_xml)
  {
    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "invalid_xml"));
  }

  xslt_result = xsltApplyStylesheet(xslt, input_xml, xslt_params);

  if (!xslt_result)
  {
    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "xslt_error"));
  }

  xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, xslt_result, xslt);

  memcpy(
      enif_make_new_binary(env, doc_txt_len, &term),
      doc_txt_ptr,
      doc_txt_len);

  xmlFreeDoc(input_xml);
  xmlFreeDoc(xslt_result);
  xsltFreeStylesheet(xslt);
  xmlFree(doc_txt_ptr);

  return enif_make_tuple2(
      env,
      enif_make_atom(env, "ok"),
      term);
}

static int load(ErlNifEnv *env, void **priv, ERL_NIF_TERM info)
{
  if (xmlMemSetup(enif_free,
                  enif_alloc,
                  enif_realloc,
                  (xmlStrdupFunc)xmlStrdup) != 0)
    return 1;

  xmlInitParser();
  xmlSubstituteEntitiesDefault(1);

  return 0;
}

static int upgrade(ErlNifEnv *env, void **priv, void **old_priv, ERL_NIF_TERM info)
{
  *priv = *old_priv;
  return 0;
}

static void unload(ErlNifEnv *env, void *priv)
{
  xsltCleanupGlobals();
  xmlCleanupParser();

  enif_free_env(env);
  enif_free(priv);

  return;
}

static ErlNifFunc funcs[] = {{"transform", 3, transform, 0}};

ERL_NIF_INIT(Elixir.XsltEx, funcs, &load, NULL, &upgrade, &unload)
