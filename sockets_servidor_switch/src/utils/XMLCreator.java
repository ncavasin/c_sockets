package utils;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Creditos al autor
 * @author https://www.developer.com/java/data/Converting-JDBC-Result-Sets-to-XML-3329001.htm
 *
 */
public class XMLCreator {

	/**
	 * Recibe un ResultSet, lo parsea y lo devuelve como 
	 * archivo XML
     * Create document using DOM api
     */
    public static Document toXMLDocument(ResultSet rs)
        throws ParserConfigurationException, SQLException
    {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document doc = builder.newDocument();

        Element results = doc.createElement("Results");
        doc.appendChild(results);

        ResultSetMetaData rsmd = rs.getMetaData();
        int colCount = rsmd.getColumnCount();

        while (rs.next())
        {
            Element row = doc.createElement("Row");
            results.appendChild(row);

            for (int i = 1; i <= colCount; i++)
            {
                String columnName = rsmd.getColumnName(i);
                Object value = rs.getObject(i);

                Element node = doc.createElement(columnName);
                node.appendChild(doc.createTextNode(value.toString()));
                row.appendChild(node);
            }
        }
        return doc;
    }
    
    /**
     * Recibe un ResultSet, lo parsea y lo devuelve como string
     * Fastest, but may have encoding issues
     */
    public static String toXMLString(ResultSet rs) throws SQLException
    {
        ResultSetMetaData rsmd = rs.getMetaData();
        int colCount = rsmd.getColumnCount();
        StringBuffer xml = new StringBuffer();
        xml.append("<Results>");

        while (rs.next())
        {
            xml.append("<Row>");

            for (int i = 1; i <= colCount; i++)
            {
                String columnName = rsmd.getColumnName(i);
                Object value = rs.getObject(i);
                xml.append("<" + columnName + ">");

                if (value != null)
                {
                    xml.append(value.toString().trim());
                }
                xml.append("</" + columnName + ">");
            }
            xml.append("</Row>");
        }
        xml.append("</Results>");

        return xml.toString();
    }


    /**
	 * Recibe un String y almacena su contenido
	 * @param toSave String con el contenido a escribir en un archivo
	 * @throws SQLException
	 */
	public static void saveStringAsXML(String toSave, String filePath) 
			throws IOException{	
		Path path = Paths.get(filePath);
	    byte[] strToBytes = toSave.getBytes();
	    Files.write(path, strToBytes);	  
	}
	
	
	/**
     * Recibe el path a un .XML Lo parsea a formato 
     * tabla y lo devuelve como string
     * @param pathToXML del archivo a parsear
     * @return string con formato tabla 
     */
    public static String xmlToString(String pathToXML) {
    	// String con el resultado a mostrar
    	String output = "";

    	// String con el nombre de las columnas
    	String nomCols = "";
    	    	
    	// Obtengo un creador de documentos XML
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = null;
		
		try {
			// Creo un nuevo builder
			builder = factory.newDocumentBuilder();
			
			// Creo un documento xml en memoria
			Document xml = builder.parse(pathToXML);
			
			// Normalizo su formato
			xml.normalize();
			
			// Recupero todas las tuplas devueltas en una lista
			NodeList tupList = xml.getElementsByTagName("Row");
			
			// Obtengo cantidad de tuplas (<Rows>) recuperadas
			int tupLen = tupList.getLength(); 
			
			if(tupLen == 0) {
				return "La query fue ejecutada pero no devolvio ningun resultado.";
			}
			
			// Recorro todas las tuplas
			for(int i=0; i < tupLen; i ++) {
				
				// Obtengo las tuplas de una en una desde la lista
				Node tupla = tupList.item(i);
				
				// Cada tupla tiene el formato: <Row>....</Row>
				// Dentro de cada tupla voy a tener pares atributo-valor
				// con el formato <nombre_atributo> valor </nombre_atributo>
				// que representan una columna 
						
				// Recorro las columnas de la tupla
				while(tupla.hasChildNodes()) {
					
					// Obtengo la primer columna
					Node col = tupla.getFirstChild();
					
					// Almaceno los nombres de cada columna
					if(i == 0) {
						nomCols += String.format("%-35s", col.getNodeName());
					}				
					
					// Formateo el string para despues 
					// mostrarlo en el textArea
					output += String.format("%-35s", col.getTextContent());
										
					// La elimino pues ya fue creada
					tupla.removeChild(col);
				}
				output += "\n";
			}			
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}		
		nomCols += String.format("%-35s", "\n-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"); 
		
		return nomCols.concat("\n"+output);
	}

    
    /**
     * Parsea un XML y devuelve una lista de strings
     * correspondiente al valor de las columnas encontradas
     * @param pathToXML
     * @return
     */
    public static ArrayList<String> xmlToList(String pathToXML) {
		ArrayList<String> output = new ArrayList<String>();
		
		// Obtengo un creador de documentos XML
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = null;
		
		try {
			// Creo un nuevo builder
			builder = factory.newDocumentBuilder();
			
			// Creo un documento xml en memoria
			Document xml = builder.parse(pathToXML);
			
			// Normalizo su formato
			xml.normalize();
			
			// Recupero todas las tuplas devueltas en una lista
			NodeList tupList = xml.getElementsByTagName("Row");
			
			// Cada tupla tiene el formato: <Row>....</Row>
			// Dentro de cada tupla voy a tener pares atributo-valor
			// con el formato <nombre_atributo> valor </nombre_atributo>
			
			// Obtengo cantidad de tuplas (<Rows>) recuperadas
			int tupLen = tupList.getLength(); 

			// Recorro todas las tuplas
			for(int i=0; i < tupLen; i ++) {
				// Obtengo una tupla
				Node tupla = tupList.item(i);
				
				// Recorro las columnas de la tupla
				while(tupla.hasChildNodes()) {
					// Obtengo la primer columna
					Node col = tupla.getFirstChild();
					
					// Almaceno su contenido
					String val = col.getTextContent();
					
					// Lo agrego a la lista
					output.add(val);
					
					// Elimino la columna pues ya fue procesada
					tupla.removeChild(col);
				}
			}			
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}		
		return output;
	}


	
	

    
}
