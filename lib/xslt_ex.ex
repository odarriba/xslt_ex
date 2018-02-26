defmodule XsltEx do
  @moduledoc """
  Module to do XSLT transformations on the fly.
  """

  @on_load {:init, 0}

  app = Mix.Project.config()[:app]

  # loading the NIF
  def init do
    path =
      unquote(app)
      |> :code.priv_dir()
      |> :filename.join('xslt_ex')

    :ok = :erlang.load_nif(path, 0)
  end

  @doc """
  Apply a given XSLT stylesheet to a binary XML

  ## Examples

      iex> XsltEx.transform("path/to/stylesheet.xslt", xml)
      %{:ok, transformed_xml}
  """
  @spec transform(xslt_path :: String.t(), xml_data :: String.t()) :: tuple()
  def transform(xslt_path, xml_data)

  def transform(_, _) do
    # if the NIF can't be loaded, this function is called instead.
    exit(:nif_library_not_loaded)
  end
end
