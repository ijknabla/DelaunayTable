within DelaunayTable;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  Types.ExternalDelaunayTable table = Types.ExternalDelaunayTable();

equation

  y[:] = zeros(nout);

end DelaunayTable;