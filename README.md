# XsltEx

[![Current Version](https://img.shields.io/hexpm/v/xslt_ex.svg)](https://hex.pm/packages/xslt_ex)

Elixir library based on [KlausTrainer/erl_xslt](https://github.com/KlausTrainer/erl_xslt) to provide native XSLT transformation capabilities.

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `xslt_ex` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:xslt_ex, "~> 0.1.0"}
  ]
end
```

## Requirements

To use this library you need to be able to compile a C software that makes use
of `libxslt` library, so:

- GCC or similar
- Make
- libxslt (including headers and source)

## Usage

To use it, just call:

```elixir
{:ok, result} = XsltEx.transform(path_to_your_xslt, xml_binary_data)
```

If something fails, it will return a tuple `{:error, reason}`.


## Disclaimer

This library uses a **NIF**, which means it can heavily break you Erlang VM if
something goes wrong.

## TODO

1. Add variables.
2. Investigate random segmentation faults.

## Contribute

All contributions are welcome, and we really hope this repo will serve for beginners as well for more advanced developers.

If you have any doubt, feel free to ask, but always respecting our [Code of Conduct](https://github.com/odarribva/xslt_ex/blob/master/CODE_OF_CONDUCT.md).

To contribute, create a fork of the repository, make your changes and create a PR. And remember, talking on PRs/issues is a must!

