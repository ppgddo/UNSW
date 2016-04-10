package com.timhale.ass1;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.xml.sax.InputSource;

/**
 * Servlet implementation class ControllerServlet
 */
@WebServlet(description = "Controller Servlet for the Music Store in Ass1", urlPatterns = { "/ControllerServlet" })
public class ControllerServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	private MusicXmlData m_musicXmlData;
    
	@Override
	public void init() throws ServletException {
		super.init();
		
		// Open the xml file
		try 
		{
			ServletContext context = getServletContext();
			InputSource xmlFile = new InputSource(context.getResourceAsStream("/WEB-INF/musicDb.xml"));
		
			m_musicXmlData = new MusicXmlData(xmlFile);
		}
		catch (Exception exc) {
			throw new ServletException(exc);
		}
	}
    
    
    
    
	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		// Get data
		// TODO: get this from XML file
		String[] artist = {"Sting", "Tim Hale", "Cia", "U2"};
		request.setAttribute("artist_list", artist);
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/results.jsp");
			
		// forward the request to JSP
		dispatcher.forward(request, response);

	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		doGet(request, response);
	}

}
