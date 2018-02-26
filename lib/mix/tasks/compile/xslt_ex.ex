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
      {result, _error_code} = System.cmd("make", ["priv/xslt_ex.so"], stderr_to_stdout: true)
      IO.binwrite(result)
    end

    :ok
  end
end
