package com.timhale.ass1;

import java.io.IOException;
import java.util.List;

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

	private MusicXmlData m_musicData;
    
	@Override
	public void init() throws ServletException {
		super.init();
		
		// Open the xml file
		try 
		{
			ServletContext context = getServletContext();
			InputSource xmlFile = new InputSource(context.getResourceAsStream("/WEB-INF/musicDb.xml"));
		
			m_musicData = new MusicXmlData(xmlFile);
		}
		catch (Exception exc) {
			throw new ServletException(exc);
		}
	}
    
    
    
    
	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		try
		{
			String control_cmd = request.getParameter("control_cmd");
			
			if (control_cmd == null) 
			{
				// The default control command will be "SEARCH"
				control_cmd = "SEARCH";
			}
			
				// route to the appropriate method
			switch (control_cmd) 
			{
			
			case "SEARCH":
				doSearch(request, response);
				break;
				
			case "RESULTS":
				showResullts(request, response);
				break;

			case "CART":
				shoppingCart(request, response);
				break;
				
			default:
				doSearch(request, response);
			}
		}
		catch (Exception exc) 
		{
			// TODO Handle this exception better!!
			doSearch(request, response);
		}
	}

	
	private void shoppingCart(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		// see which check boxes are checked
		String[] checkboxNamesList= request.getParameterValues("add");
		

		
		for (int i = 0; i < checkboxNamesList.length; i++) 
		{
			
			System.out.println(checkboxNamesList[i]);

			/*
		    String myCheckBoxValue = request.getParameterValues(checkboxNamesList[i]);

		    // if null, it means checkbox is not in request, so unchecked 
		    if (myCheckBoxValue == null)
		    	System.out.println(checkboxNamesList[i] + "=unchecked");

		    // if is there, it means checkbox checked
		    else
		    	System.out.println(checkboxNamesList[i] + "=checked");
		    	*/

		}		
		
		
		
		
		
		
		
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/shopping_cart.jsp");
			
		// forward the request to JSP
		dispatcher.forward(request, response);
		
	}




	private void showResullts(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// Get data
		// TODO: get this from XML file
		
		// get the search text from form data
		String searchString = request.getParameter("searchText");
		
		// TODO do a search for the search string
		
		//String[] artist = {searchString, "Sting", "Tim Hale", "Cia", "U2"};
		//request.setAttribute("artist_list", artist);
		
		// TODO remove this test code
		List<Song> songList = m_musicData.GetRandomSongs(10);
		request.setAttribute("SONG_LIST", songList);

		
		
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/results.jsp");
			
		// forward the request to JSP
		dispatcher.forward(request, response);

	}




	private void doSearch(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/search.jsp");

		// Get the list of 10 random songs
		List<Song> songList = m_musicData.GetRandomSongs(10);
		request.setAttribute("SONG_LIST", songList);
		
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
