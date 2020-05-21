within DelaunayTable;

block DelaunayTable
  extends Modelica.Blocks.Interfaces.MIMO;

  parameter String tableName = "NoName";
  parameter String fileName = "NoName"
    "File where table is stored"
  annotation (
    Dialog(
      loadSelector(
        filter="Text files (*.txt);;",
        caption="Open file in which table is present"
      )
    )
  );

  parameter Integer verbosity(min=0) = 0;

  Types.ExternalDelaunayTable table = Types.ExternalDelaunayTable(
    tableName = tableName,
    fileName = fileName,
    verbosity = verbosity
  );

equation

  y[:] = zeros(nout);

end DelaunayTable;