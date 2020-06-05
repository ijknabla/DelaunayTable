within DelaunayTables;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  parameter Real[:,nin+nout] table;

  parameter Types.Verbosity verbosity = Types.Verbosity.quiet;

  Types.ExternalDelaunayTable tableObject = Types.ExternalDelaunayTable(nin, nout, table, verbosity);

protected

  function get_value
    extends Modelica.Icons.Function;
    input Types.ExternalDelaunayTable self;
    input Integer nout;
    input Real[:] u;
    input Types.Verbosity verbosity;
    output Real[nout] y;
  external "C" ExternalDelaunayTable__get_value(
    self,
    size(u, 1),
    nout,
    u,
    y,
    verbosity
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
   );
  end get_value;

equation

  y = get_value(tableObject, nout, u, verbosity);

end DelaunayTable;
