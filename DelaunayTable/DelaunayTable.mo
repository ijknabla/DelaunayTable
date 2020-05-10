within DelaunayTable;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  parameter String tableName = "NoName";
  parameter String fileName = "NoName";

  Types.ExternalDelaunayTable table = Types.ExternalDelaunayTable();

equation

  y[:] = zeros(nout);

end DelaunayTable;