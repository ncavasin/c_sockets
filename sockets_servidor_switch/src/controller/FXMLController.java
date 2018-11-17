package controller;


import java.net.URL;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.layout.VBox;
import model.Model;
import utils.DAO;
import utils.XMLCreator;

/***
 * 
 * @author nicolas cavasin
 *
 */
public class FXMLController {
	
	private Model model;
	
	public Map<String, DAO> myEngines;
	
    @FXML private ResourceBundle resources;

    @FXML private URL location;
    
    // ---------------------------- ESTRUCTURA -----------------------------------
    
    @FXML private ComboBox<String> dbComboBox;

    @FXML private ComboBox<String> engineComboBox;
    
    @FXML private TextField txtSQL;
   
    @FXML private TextArea txtArea;
    
    @FXML private TextField txtTable;

    @FXML private Button refreshBtn;

    @FXML private VBox vboxMeta;
    
    @FXML private VBox vboxEng;
    
    @FXML private ListView<String> listTbl;
        
    // ---------------------------- FUNCIONALIDAD --------------------------------
           
    /**
     * Se acciona cuando se presiona "btnExecuteSQL". Obtiene el 
     * contenido de "txtSQL" y se lo envia al modelo para que lo 
     * ejecute. El resultado se refleja en txtArea.
     */
    @FXML void executeSQL(ActionEvent event) {
    
    	// Almacena el nombre del motor
    	String engName = "";
    	
    	// Almacena el nombre de la bd 
		String dbName;
    	
    	// Almacena el nombre del XML que contendra el resultado
    	String resultXML="";
    	
    	// Obtengo el sql escrito en el text field
    	String sql = txtSQL.getText();
    	
    	// Verifico que no este vacio
    	if(! sql.isEmpty()) {
    		
    		// Si no esta vacio, envio a ejecutar el SQL al modelo
    		try {

    			// Obtengo el motor seleccionado
    			engName = engineComboBox.getSelectionModel().getSelectedItem();
    			
    			// Obtengo la db seleccionada
    			dbName = dbComboBox.getSelectionModel().getSelectedItem();
				
    			// Seteo la base seleccionada para que se conecte
    			myEngines.get(engName).setDbName(dbName);
    			
    			System.out.println("[CONTROLLER.FXMLCONTROLLER]" + myEngines.get(engName).getUrl());
    			
    			// Mando a ejecutar la query indicada
    	    	resultXML = model.doSQL(myEngines.get(engName), sql, "executeSQL");

    	    	// Vacio el contenido del txtField
    	    	txtSQL.setText("");
    			txtSQL.setPromptText("SQL a ejecutar + Enter");
    			
    			// Refresco comboBox de DBs
    			getDbsByEngine(engName);
    			
    			// Refresco list view de tablas
    			getTablesByDb(engName, dbName);
    			
    			// Si hubo algun resultado
    			if(! resultXML.isEmpty()) {

    				// Parseo el resultado
    				String res = XMLCreator.xmlToString(resultXML);
    				
    				// Lo muestro en el textArea
    				if(res.length() == 0){
    					txtArea.setText("La query fue ejecutada pero no devolvio ningun resultado.");
    				}else {
    					
        				txtArea.setText(res);	
    				}
    			}
				// Atrapo errores de sintaxis y demas
			} catch (SQLException e1) {
				// Informo por consola 
				System.out.println("[" + FXMLController.class.getName().toUpperCase() + "] - " + e1.getMessage());
			}catch(Exception e2) {
				System.out.println(e2.getMessage());
			}
    	}    	
    }

    
	/**
     * Levanta de un XML las dbs disponibles del motor
     * que ha sido seleccionado en el engineComboBox
     * y las carga en dbComboBox
     * @param event
     */
	@FXML void engineBoxSelection(ActionEvent event) {
    	// Almacena el nombre de motor seleccionado
    	String engName = "";
    	
    	// Obtengo el motor seleccionado
		engName = engineComboBox.getSelectionModel().getSelectedItem();
		
		if(engName == null) {
			return;
		}
		
		System.out.println("MOTOR ELEGIDO: "+engName);

		// Obtenido el motor seleccionado, debo ir a buscar las 
		// dbs que este contiene para cargarlas en el combo box
		// "dbComboBox"
		
		// Como ya fueron cargadas al inicar la app, solo debo
		// parsear el xml asociado a ese motor para levantar su
		// contenido. Esto lo hago concatenando el string
		// "Databases.xml" al nombre de motor. Ejemplos: 
		// "PostgresDatabases.xml" o "MySQLDatabases.xml"    	
		String xmlDbs = "src/results/" + engName.concat("Databases.xml");
		
		// Parseo ese xml y devuelvo una lista de dbs que tiene el motor
		ArrayList<String> listadb = XMLCreator.xmlToList(xmlDbs);

		// Vacio el contenido previo de dbComboBox
		dbComboBox.getItems().remove(0, dbComboBox.getItems().size());
		
		// Cargo todas las dbs del motor en el combo box
		for(String db : listadb) {
			dbComboBox.getItems().add(db);
			System.out.println(db);
		}
		
		// Habilito el combo de databases
		dbComboBox.setPromptText("Databases");
		dbComboBox.setVisible(true);
		
		// Habilito el vbox de motor
		vboxEng.setVisible(true);
		
		// Vacio lista de tablas
		listTbl.getItems().remove(0, listTbl.getItems().size());
		
    }
 
	
    /**
     * Obtiene la db seleccionada en dbComboBox
     * y muestra en listView todas las tablas que 
     * esta tenga
     * @param event
     */
    @FXML void dbBoxSelection(ActionEvent event) {
    	// Almacen a el nombre de la db seleccionada
    	String dbName = "";
    	
    	// Almacen a el nombre del motor seleccionado
    	String engName = "";
    	
    	// Obtengo el motor seleccionado otra vez
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("MOTOR ELEGIDO: "+ engName);
    	
    	// Obtengo la db seleccionada
    	dbName = dbComboBox.getSelectionModel().getSelectedItem();

    			System.out.println("DB ELEGIDA: "+ dbName);
    			
    	if(dbName == null) {
    		return;
    	}
    	
    	// Obtenida la db seleccionada, debo ir a buscar 
    	// sus tablas para mostrarla en el txtArea
    	// Esto lo hago concatenando el string 
    	// "Tables.xml" al nombre del motor. Ejemplos:
    	// "PostgresTables.xml" o "MySQLTables.xml"
		getTablesByDb(engName, dbName);
		
		// Habilito el boton de refresh
		refreshBtn.setVisible(true);
		
		// Habilito el text SQL
		txtSQL.setVisible(true);
		
		// Habilito el toolbar de database
		vboxMeta.setVisible(true);
    }

    
    @FXML void showProcedures(ActionEvent event) {
    	// Almacena el motor seleccionado
    	String engName = "";
    	
    	// Almacena la database seleccionada
    	String dbName = "";
    	
    	// Almacena el path del XML con el resultado
    	String resultXML = "";
    	
    	// Obtengo el motor seleccionado otra vez
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showProcedures - MOTOR ELEGIDO: "+ engName);
    	
    	// Obtengo la db seleccionada
    	dbName = dbComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showProcedures - DB ELEGIDA: "+ dbName);
    			
    	if(engName == null || dbName == null) {
    		return;
    	}
    	
    	// Seteo la db seleccionada para que se conecte
    	myEngines.get(engName).setDbName(dbName);
    	
    	try {
    		// Obtengo todos los procedimientos almacenados en la db
			resultXML = model.getProcedures(myEngines.get(engName), dbName);
			
			// Si obtuve algun resultado
			if(!resultXML.isEmpty()){
				// Lo muestro en el text area
				txtArea.setText(XMLCreator.xmlToString(resultXML));
			}else {
				txtArea.setText(dbName + " no tiene procedimientos almacenados.");
			}
			
		} catch (SQLException e) {System.out.println(e.getMessage());}
    }
    
    
    @FXML void showTriggers(ActionEvent event) {
    	// Almacena el motor seleccionado
    	String engName = "";
    	
    	// Almacena la database seleccionada
    	String dbName = "";
    	
    	// Almacena el path del XML con el resultado
    	String resultXML = "";
    	
    	// Obtengo el motor seleccionado otra vez
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showTriggers - MOTOR ELEGIDO: "+ engName);
    	
    	// Obtengo la db seleccionada
    	dbName = dbComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showTriggers - DB ELEGIDA: "+ dbName);
    			
    	if(engName == null || dbName == null) {
    		return;
    	}
    	
    	// Seteo la db seleccionada para que se conecte
    	myEngines.get(engName).setDbName(dbName);

    	try {
    		// Obtengo todos los procedimientos almacenados en la db
			resultXML = model.getTriggers(myEngines.get(engName), dbName);
			
			// Si obtuve algun resultado
			if(!resultXML.isEmpty()){
				// Lo muestro en el text area
				txtArea.setText(XMLCreator.xmlToString(resultXML));
			}else {
				txtArea.setText(dbName + " no tiene triggers almacenados.");
			}
			
		} catch (SQLException e) {System.out.println(e.getMessage());}
    }
    
    
    @FXML void showUsers(ActionEvent event) {
    	// Almacena el motor seleccionado
    	String engName = "";
    	   	
    	// Almacena el path del XML con el resultado
    	String resultXML = "";
    	
    	// Obtengo el motor seleccionado 
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showUsers - MOTOR ELEGIDO: "+ engName);
    			
    	if(engName == null) {
    		return;
    	}
    	    	
    	try {
    		// Obtengo todos los procedimientos almacenados en la db
			resultXML = model.getUsers(myEngines.get(engName));
			
			// Si obtuve algun resultado
			if(!resultXML.isEmpty()){
				
				// Lo muestro en el text area
				txtArea.setText(XMLCreator.xmlToString(resultXML));
			}else {
				txtArea.setText(engName + " no tiene usuarios.");
			}
			
		} catch (SQLException e) {System.out.println(e.getMessage());}
    }
    
    
    @FXML void showMetadata(ActionEvent event) {
    	// Almacena el motor seleccionado
    	String engName = "";
    	
    	// Almacena la database seleccionada
    	String dbName = "";
    	
    	// Almacena la tabla indicada
    	String tbName = "";
    	
    	// Almacena el path del XML con el resultado
    	String resultXML = "";
    	    	
    	// Obtengo el motor seleccionado otra vez
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showMetadata - MOTOR ELEGIDO: "+ engName);
    	
    	// Obtengo la db seleccionada
    	dbName = dbComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showMetadata - DB ELEGIDA: "+ dbName);
    			
    	// Obtengo la tb seleccionada
    	tbName = listTbl.getSelectionModel().getSelectedItem();
    			System.out.println("showTriggers - TBL ELEGIDA: "+ tbName);
    	
    	if(engName == null || dbName == null || tbName == null) {
    		System.out.println("SALIENDO SHOWMETADATA!");
    		return;
    	}
    	
    	// Seteo la db seleccionada para que se conecte
    	myEngines.get(engName).setDbName(dbName);

    	try {
    		// Obtengo todos los triggers de la tabla
			resultXML = model.getTablaMetadata(myEngines.get(engName), tbName);
			
			// Si obtuve algun resultado
			if(!resultXML.isEmpty()){
				
				// Lo muestro en el text area
				txtArea.setText(XMLCreator.xmlToString(resultXML));	
			}
		} catch (SQLException e) {System.out.println(e.getMessage());}

    }
    
    
    @FXML void showTableTriggers(ActionEvent event) {
    	// Almacena el motor seleccionado
    	String engName = "";
    	
    	// Almacena la database seleccionada
    	String dbName = "";
    	
    	// Almacena la tabla indicada
    	String tbName = "";
    	
    	// Almacena el path del XML con el resultado
    	String resultXML = "";
    	    	
    	// Obtengo el motor seleccionado otra vez
    	engName = engineComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showTriggers - MOTOR ELEGIDO: "+ engName);
    	
    	// Obtengo la db seleccionada
    	dbName = dbComboBox.getSelectionModel().getSelectedItem();
    			System.out.println("showTriggers - DB ELEGIDA: "+ dbName);
    			
    	// Obtengo la tb seleccionada
    	tbName = listTbl.getSelectionModel().getSelectedItem();
    	
    	if(engName == null || dbName == null || tbName == null) {
    		return;
    	}
    	
    	// Seteo la db seleccionada para que se conecte
    	myEngines.get(engName).setDbName(dbName);

    	try {
    		// Obtengo todos los triggers de la tabla
			resultXML = model.getTriggersTabla(myEngines.get(engName), tbName);
			
			// Si obtuve algun resultado
			if(!resultXML.isEmpty()){
				
				// Lo muestro en el text area
				txtArea.setText(XMLCreator.xmlToString(resultXML));
			}else {
				txtArea.setText(tbName + " no tiene triggers almacenados.");
			}
			
		} catch (SQLException e) {System.out.println(e.getMessage());}
    }
       
    /**
     * Obtiene nuevamente todos los motores disponibles
     * y todas las DBs de cada motor.
     * Luego los llena en los combo boxes que corresponde
     * @param event click en el boton de refresh
     */
    @FXML void refreshComboBoxes(ActionEvent event) {
    	loadEngines();
    	txtArea.setText("");
    }
        
    // ---------------------------------------- I'M A SPLITTER ----------------------------------------
    
	
	/**
	 * Crea e inicializa mapa de motores
	 * Obtiene XMLs de dbs disponibles en cada motor
	 * Carga el combo box de motores
	 */
	private void loadEngines() {
		
		// Inicializo el combo box de motores
    	if(engineComboBox.getItems().size() > 0) {
    		engineComboBox.getItems().remove(0, engineComboBox.getItems().size());
    	}
    	
		// Obtengo los iteradores tanto de las claves como de los valores
        Iterator<DAO> values = myEngines.values().iterator();
        Iterator<String> keys = myEngines.keySet().iterator();
        
        // Cargo los datos de conexion utilizando los iteradores
        while(keys.hasNext()) {
        	// Almacena DAOs
        	DAO d = (DAO) values.next();
        	
        	// Almacena las claves de cada DAO
        	String engineName = keys.next();
        	
        	// Formateo para utilizar la clave como nombre del archivo
        	// del cual se deben obtener los datos de conexion
        	d.loadProps(engineName + ".properties");
        	
        	// Agrego el motor al combo box de motores
        	engineComboBox.getItems().add(engineName);
        	        	
        	// Obtengo las DBs del motor
        	getDbsByEngine(engineName);
        }
        
        // Muestro el engineComboBox
        engineComboBox.setPromptText("Motores");
        engineComboBox.setVisible(true);
        
        // Deshabilito el combo box de las dbs
        dbComboBox.setPromptText("Databases");
        dbComboBox.setVisible(false);
        
        // Deshabilito el boton de refresh
        refreshBtn.setVisible(false);
        
        // Deshabilito el texto SQL
        txtSQL.setVisible(false);
        
        // Deshabilito el toolbar de database
     	vboxMeta.setVisible(false);
     	
     	// Deshabilito el toolbar de motor
     	vboxEng.setVisible(false);
        
	}

	
	/**
	 * Almacena en un XML las DBs del motor recibido 
	 * @param engineName
	 */
	private void getDbsByEngine(String engineName) {
		// Obtengo las databases que tiene el motor recibido
    	try {
			model.getDatabases(myEngines.get(engineName));
		} catch (SQLException e) {/* no puedo hacer nada para solucionarlo*/}	
		
	}
	
	 
    /**
     * Recibe un motor y una db. Almacena las tablas de la
     * db recibida en un XML
     * @param engName nombre del motor
     * @param dbName nombre de la DB
     */
    private void getTablesByDb(String engName, String dbName) {
    	// Obtenida la db seleccionada, debo ir a buscar 
    	// sus tablas para mostrarla en el txtArea
    	// Esto lo hago concatenando el string 
    	// "Tables.xml" al nombre del motor. Ejemplos:
    	// "PostgresTables.xml" o "MySQLTables.xml"
    	try {
	    	
    		// Seteo la base seleccionada para que se conecte
			myEngines.get(engName).setDbName(dbName);
    		
			// Obtengo las tablas de la db seleccionada
			model.getTablas(myEngines.get(engName), dbName);
			
			// Obtengo el path del XML donde se almacenan
			String xmlTbls = "src/results/" +engName.concat("Tables.xml");
			
			// Levanto las tablas del XML
			ArrayList<String> list = XMLCreator.xmlToList(xmlTbls);
			
			// Remuevo lo que haya
			listTbl.getItems().remove(0, listTbl.getItems().size());
			
			// Las agrego al list view
			for(String tbl : list) {
				listTbl.getItems().add(tbl);
			}
				
			
			} catch (SQLException e) { /* no puedo hacer nada */}
		
	}

	
	// ---------------------------------------- I'M A SPLITTER ----------------------------------------
	
	@FXML void initialize() {
          
        // Instancio el modelo
        model = new Model();

        // Instancio el mapa de los motores
        myEngines = new HashMap<String, DAO>();
              
        // Creo los motores y los agrego al mapa
        //myEngines.put("Firebird", new DAO());
        myEngines.put("MySQL", new DAO());
        myEngines.put("Postgres", new DAO()); 
                
        // Obtengo las dbs de todos los motores
        loadEngines();        
   
    }
	
    
}
