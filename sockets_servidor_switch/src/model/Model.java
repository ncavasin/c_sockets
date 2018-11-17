package model;

import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import utils.DAO;
import utils.XMLCreator;

/**
 * @author nicolas cavasin
 *
 */
public class Model {
	private final String doQueryPath = "src/results/";
	
	public Model() {
		super();
	}

	/**
	 * Recibe SQL cuya ejecucion SI devuelve un resultado (SELECTs).
	 * El SQL es ejecutado sobre la referencia DAO
	 * @param query a ejecutar en la BD
	 * @throws SQLException 
	 * @throws IOException 
	 */
	public String doSQL(DAO d, String sql, String resultFileName) throws SQLException{
		
		// Conexion a la BD
		Connection cx = null;
		
		// Utilizado para ejecutar la query
		Statement st = null;
		
		// Almacena el resultado de la query
		ResultSet rs = null;

		try{
			
			// Obtengo conexion a la BD
			cx = d.getConnection();
			
			// Me preparo para ejecutar el SQL
			st = cx.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
			
			// Ejecuto el SQL y almaceno su resultado
			rs = st.executeQuery(sql);
			
			if(rs != null ) {
				// Verifico que el path del XML que contendra el resultado no sea nulo
				if(resultFileName.isEmpty()) {
					resultFileName = doQueryPath + "doQuery.xml";
				}
				
				// Le doy formato al nombre de archivo
				resultFileName = doQueryPath + resultFileName + ".xml";

				// Lo guardo en el archivo XML
				XMLCreator.saveStringAsXML(XMLCreator.toXMLString(rs), resultFileName);
			}	
			
			//No cierro el Statement porque sino tambien se cierra el ResulSet
			// Es un comportamiento automatico y no busque como solucionarlo

			// Cierro la conexion
			if(cx != null) {
				cx.close();
			}	
			
			// Atrapo errores en la ejecucion de la query o en la escritura del XML
			// Si ocurrio, lo elevo para que lo administre el controlador
		} catch (SQLException e1) { // Excepcion en ejecucion
			System.out.println("[" + Model.class.getName().toUpperCase() + "] - " + e1.getMessage());
		} catch (IOException e2) { // Excepcion en escritura XML
			System.out.println("[" + Model.class.getName().toUpperCase() + "] - " + e2.getMessage());
		}

		return resultFileName;
	}

	/**
	 * Obtiene las bds almacenadas en el motor d
	 * @param d objeto DAO
	 * @throws IOException 
	 * @throws SQLException 
	 */
	public String getDatabases(DAO d) throws SQLException {
		String sql = "";
		
		// Metadata de motor
		sql = d.getProps().getProperty("qr.dbs");
		return doSQL(d, sql, d.getProps().getProperty("qr.dbs.res"));
	}
	
	/**
	 * Obtiene las tablas almacenadas en la bd recibida
	 * @param d objeto DAO
	 * @param db nombre de la bd
	 * @throws  
	 * @throws SQLException 
	 */
	public String getTablas(DAO d, String db) throws SQLException {
		String sql = "";
		
		// Metadata de "db"
		sql = d.getProps().getProperty("qr.tbl");
		sql = sql.replaceAll("NOMBREBD", db);
		return doSQL(d, sql, d.getProps().getProperty("qr.tbl.res"));
	}
	
	
	/**
	 * Obtiene metadatos sobre los atributos de la tabla recibida
	 * @param d objeto DAO
	 * @param table nombre de la tabla
	 */
	public String getTablaMetadata(DAO d, String table) throws SQLException {
		String sql = "";
		
		// Metadata de "table"
		sql = d.getProps().getProperty("qr.atrmet");
		sql = sql.replaceAll("NOMBRETABLA", table);
		return doSQL(d, sql, d.getProps().getProperty("qr.atrmet.res"));
	}
	
	
	/**
	 * Obtiene el contenido de la tabla recibida
	 * @param d objeto DAO
	 * @param table nombre de la tabla
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getAtributos(DAO d, String table) throws SQLException  {
		String sql = "";
		
		// Contenido de "table"
		sql = d.getProps().getProperty("qr.atr");
		sql = sql.replaceAll("NOMBRETABLA", table);
		return doSQL(d, sql, d.getProps().getProperty("qr.atr.res"));
	}
	
	/***
	 * Lista los triggers almacenados en la bd  recibida
	 * @param d objeto  DAO
	 * @param db nombre de la bd
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getTriggers(DAO d, String db) throws SQLException {
		String sql = "";
		
		// Triggers almacenados en "db"
		sql = d.getProps().getProperty("qr.trgbd");
		sql = sql.replaceAll("NOMBREBD", db);
		return doSQL(d, sql, d.getProps().getProperty("qr.trgbd.res"));
	}
	
	/***
	 * Lista los triggers asociados a la tabla recibida
	 * @param d objeto  DAO
	 * @param table nombre de la tabla
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getTriggersTabla(DAO d, String table) throws SQLException {
		String sql = "";
		
		// Triggers asociadaos a "table"
		sql = d.getProps().getProperty("qr.trgtbl");
		sql = sql.replaceAll("NOMBRETABLA", table);
		return doSQL(d, sql, d.getProps().getProperty("qr.trgtbl.res"));
	}
	
	/**
	 * Muestra el codigo fuente del trigger recibido 
	 * @param d objeto  DAO
	 * @param trgName nombre del trigger
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getTriggerSource(DAO d, String trgName) throws SQLException {
		String sql = "";
		
		// Codigo fuente del trigger "trgName"
		sql = d.getProps().getProperty("qr.trgsrc");
		sql = sql.replaceAll("NOMBRETRIGGER", trgName);
		return doSQL(d, sql, d.getProps().getProperty("qr.trgsrc.res"));
	}
	

	/**
	 * Lista los procedimientos almacenados en la bd recibida
	 * @param d objeto  DAO
	 * @param db nombre de la bd
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getProcedures(DAO d, String db) throws SQLException {
		String sql = "";
		
		// Procedimientos almacenados en "db"
		sql = d.getProps().getProperty("qr.prcbd");
		sql = sql.replaceAll("NOMBREBD", db);
		return doSQL(d, sql, d.getProps().getProperty("qr.prcbd.res"));
	}

	/**
	 * Muestra el codigo fuente del procedure recibido
	 * @param d objeto  DAO
	 * @param prcName nombre del procedimiento
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getProcedureSource(DAO d, String prcName) throws SQLException {
		String sql = "";
		
		// Codigo fuente del procedure "prcName"
		sql = d.getProps().getProperty("qr.prcsrc");
		sql = sql.replaceAll("NOMBREPROCEDURE", prcName);
		return doSQL(d, sql, d.getProps().getProperty("qr.prcsrc.res"));
	}

	/**
	 * Lista todas las secuencias del motor 
	 * @param d objeto DAO
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getSequences(DAO d) throws SQLException {
		String sql = "";
		
		// Lista todas las secuencias del motor "d"
		sql = d.getProps().getProperty("db.seq");
		return doSQL(d, sql, d.getProps().getProperty("db.seq.res"));
	}
	
	/**
	 * Lista todos los usuarios del motor 
	 * @param d objeto DAO
	 * @return path del XML que almacena el resultado de la query
	 * @throws SQLException
	 */
	public String getUsers(DAO d) throws SQLException {
		String sql = "";
		
		// Lista todos los usuarios del motor "d"
		sql = d.getProps().getProperty("qr.usr");
		return doSQL(d, sql, d.getProps().getProperty("qr.usr.res"));
	}
	

}
