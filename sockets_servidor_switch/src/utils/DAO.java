/**
 * 
 */
package utils;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 * @author nicolas cavasin
 *
 */
public class DAO{
	private Properties 		props 	= null;
	private String 			filePath;	// Donde se almacenan las properties
	private String 			url;
	private String			user;
	private String 			password;
	
	public DAO () {
		super();
	}
	
	/**
	 * Obtiene los datos de la BD a conectarse
	 * @param path	al archivo que tiene los datos de conexion
	 */
	public void loadProps(String path) {
		props = new Properties();
		filePath = path;

		// Leo el contenido de filePath
		try (InputStream is = this.getClass().getClassLoader().getResourceAsStream(filePath)) {
		    
			props.load(is);
		
		    // Atrapo errores en la lectura del archivo
		} catch (IOException e) {
			
			// Lo informo por consola
			System.out.println("[" + DAO.class.getName().toUpperCase() + "] - " + e.getMessage());
		}
		
		// Asigno los valores leidos
		url = props.getProperty("db.url");
		user = props.getProperty("db.user");
		password = props.getProperty("db.pass");		
	}
	
	/**
	 * Establece una conexion con la database
	 * @return un nuevo objeto Connection 
	 */
	public Connection getConnection() throws SQLException{
		// Creo el enlace con la db
		try {
			return DriverManager.getConnection(url, user, password);
			// Atrapo errores en la conexion
		} catch (SQLException e) {
			// Lo informo por consola
			System.out.println("[" + DAO.class.getName().toUpperCase() + "] - " + e.getMessage());
			// Lo elevo para que sean administrados por el controlador
			throw e;
		}	
	}

	public Properties getProps() {
		return props;
	}

	public String getUrl() {
		return url;
	}

	public String getUser() {
		return user;
	}

	public String getPassword() {
		return password;
	}

	public void setDbName(String dbName) {
		removeDbName();
		this.url = this.url.concat(dbName);
	}
	
	private void removeDbName() {
		int pos = this.url.lastIndexOf('/');
		this.url = this.url.substring(0, pos+1);
	}
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((filePath == null) ? 0 : filePath.hashCode());
		result = prime * result + ((password == null) ? 0 : password.hashCode());
		result = prime * result + ((props == null) ? 0 : props.hashCode());
		result = prime * result + ((url == null) ? 0 : url.hashCode());
		result = prime * result + ((user == null) ? 0 : user.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		DAO other = (DAO) obj;
		if (filePath == null) {
			if (other.filePath != null)
				return false;
		} else if (!filePath.equals(other.filePath))
			return false;
		if (password == null) {
			if (other.password != null)
				return false;
		} else if (!password.equals(other.password))
			return false;
		if (props == null) {
			if (other.props != null)
				return false;
		} else if (!props.equals(other.props))
			return false;
		if (url == null) {
			if (other.url != null)
				return false;
		} else if (!url.equals(other.url))
			return false;
		if (user == null) {
			if (other.user != null)
				return false;
		} else if (!user.equals(other.user))
			return false;
		return true;
	}

}









