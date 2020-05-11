within DelaunayTable.Types;

class ExternalDelaunayTable

  extends ExternalObject;

  function constructor
    extends Modelica.Icons.Function;
    input String tableName "Table name";
    input String fileName "File name";
    input Integer verbosity "Verbosity {0: silent}";
    output ExternalDelaunayTable externalDelaunayTable;
  external "C" externalDelaunayTable = DelaunayTable__open(
      tableName,
      fileName,
      verbosity
    ) annotation (
      IncludeDirectory = "modelica://DelaunayTable/Resources/C-Sources",
      Include = "#include \"DelaunayTable.c\""
    );
  end constructor;

  function destructor
    extends Modelica.Icons.Function;
    input ExternalDelaunayTable externalDelaunayTable;
  external "C" DelaunayTable__close(
      externalDelaunayTable
    ) annotation (
      IncludeDirectory = "modelica://DelaunayTable/Resources/C-Sources",
      Include = "#include \"DelaunayTable.c\""
    );
  end destructor;

end ExternalDelaunayTable;