defmodule Mix.Tasks.Compile.XsltEx do
  def run(_) do
    if match?({:win32, _}, :os.type()) do
      # libpostal does not support Windows unfortunately.
      IO.warn("Windows is not supported.")
      exit(1)
    else
      File.mkdir_p("priv")
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
      start_permanent: Mix.env() == :prod,
      deps: deps(),
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
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end
