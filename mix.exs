defmodule Mix.Tasks.Compile.XsltEx do
  def run(_) do
    if match?({:win32, _}, :os.type()) do
      # libpostal does not support Windows unfortunately.
      IO.warn("Windows is not supported.")
      exit(1)
    else
      {result, _error_code} = System.cmd("make", ["priv/xslt_ex.so"], stderr_to_stdout: true)
      IO.binwrite(result)
    end

    :ok
  end
end

defmodule XsltEx.MixProject do
  use Mix.Project

  def project do
    [
      app: :xslt_ex,
      version: "0.1.0",
      elixir: "~> 1.4",
      build_embedded: Mix.env() == :prod,
      start_permanent: Mix.env() == :prod,
      description: description(),
      package: package(),
      deps: deps(),
      docs: docs(),
      compilers: [:xslt_ex] ++ Mix.compilers()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ex_doc, ">= 0.0.0", only: :dev}
    ]
  end

  defp docs do
    [
      main: "readme",
      source_url: "https://github.com/odarriba/xslt_ex",
      extras: ["README.md"]
    ]
  end

  defp description do
    """
    Elixir library to do XSLT transformations on the fly using native libxslt.
    """
  end

  defp package do
    # These are the default files included in the package
    [
      name: :xslt_ex,
      files: ["lib", "src", "Makefile", "mix.exs", "README*", "LICENSE*", "CODE_OF_CONDUCT*"],
      maintainers: ["Óscar de Arriba"],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/odarriba/xslt_ex"}
    ]
  end
end
