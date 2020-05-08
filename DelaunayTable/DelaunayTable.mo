within DelaunayTable;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

equation

  y[:] = zeros(nout);

end DelaunayTable;