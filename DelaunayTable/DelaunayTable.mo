within DelaunayTable;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  parameter String tableName = "NoName";
  parameter String fileName = "NoName";

  parameter Integer verbosity(min=0) = 0;

  Types.ExternalDelaunayTable table = Types.ExternalDelaunayTable(
    tableName = tableName,
    fileName = fileName,
    verbosity = verbosity
  );

equation

  y[:] = zeros(nout);

end DelaunayTable;