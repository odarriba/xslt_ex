#include "xslt_ex.h"

static unsigned char *binary_to_string(ErlNifBinary *bin)
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

static ERL_NIF_TERM transform(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  ERL_NIF_TERM term;
  ErlNifBinary arg0, arg1;

  unsigned char *xslt_filename;
  xsltStylesheetPtr xslt;

  unsigned char *input_xml_string;
  xmlDocPtr input_xml, xslt_result;
  xmlChar *doc_txt_ptr;
  int doc_txt_len;

  if (argc < 2 || !enif_inspect_binary(env, argv[0], &arg0) || !enif_inspect_binary(env, argv[1], &arg1))
    return enif_make_badarg(env);

  xslt_filename = binary_to_string(&arg0);
  input_xml_string = binary_to_string(&arg1);

  if (!xslt_filename || !input_xml_string)
    return enif_make_tuple2(
        env,
        enif_make_atom(env, "error"),
        enif_make_atom(env, "enomem"));

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

  xslt_result = xsltApplyStylesheet(xslt, input_xml, NULL);

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

static ErlNifFunc funcs[] = {{"transform", 2, transform, 0}};

ERL_NIF_INIT(Elixir.XsltEx, funcs, &load, NULL, &upgrade, &unload)
