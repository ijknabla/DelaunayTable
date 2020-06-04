within DelaunayTables;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  parameter Real[:,nin+nout] table;

  Types.ExternalDelaunayTable tableObject = Types.ExternalDelaunayTable(nin, nout, table);

equation

  y[:] = zeros(nout);

end DelaunayTable;