package com.timhale.ass1;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.xml.sax.InputSource;

import com.timhale.ass1.MusicXmlData.SearchMode;

/**
 * Servlet implementation class ControllerServlet
 */
@WebServlet(description = "Controller Servlet for the Music Store in Ass1", urlPatterns = { "/ControllerServlet" })
public class ControllerServlet extends HttpServlet 
{
	// private data;
	private static final long serialVersionUID = 1L;
	private HashMap<String,Album> albumCart;
	private HashMap<String,Song> songCart;
	private MusicXmlData musicData;
    
	@Override
	public void init() throws ServletException {
		super.init();
		
		// Open the xml file
		try 
		{
			ServletContext context = getServletContext();
			InputSource xmlFile = new InputSource(context.getResourceAsStream("/WEB-INF/musicDb.xml"));
		
			albumCart = new HashMap<String, Album>();
			songCart = new HashMap<String, Song>();
			musicData = new MusicXmlData(xmlFile);
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
			case "REMOVE":
				removeFromShoppingCart(request, response);
				break;
			case "CHECKOUT":
				checkout(request, response);
				break;
			case "CONFRIM":
				confrim(request, response, true);
				break;
			case "CANCEL":
				confrim(request, response, false);
				break;
			default:
				doSearch(request, response);
			}
		}
		catch (Exception exc) 
		{
			// TODO Handle this exception better!!
			System.out.println("Error: Controller Servlet got the following error: " +
					exc.getMessage() );
			doSearch(request, response);
		}
	}

	
	private void confrim(HttpServletRequest request, HttpServletResponse response, boolean confirm)  
			throws ServletException, IOException 
	{
		// Reset all of the item data
		albumCart.clear();
		songCart.clear();
		
		RequestDispatcher dispatcher = 
				request.getRequestDispatcher("/just_msg_page.jsp");
	
		if( confirm)
		{
			request.setAttribute("MESSAGE", 
					"Thank Your for Purchasing.");
		}
		else
		{
			request.setAttribute("MESSAGE", 
					"Thank Your for Shopping.");
		}

		// forward the request to JSP
		dispatcher.forward(request, response);
		
	}




	private void checkout(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
	{
		boolean noSongsOrAlbumsOnList = GetCartListAndTotalPrice(request);
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/checkout.jsp");
		
		if(noSongsOrAlbumsOnList )
		{
			 // if both list at empty, show the "Chart is empty" page
			request.setAttribute("MESSAGE", 
					"Chart is empty!");
			dispatcher = request.getRequestDispatcher("/msg_and_back_to_search.jsp");
		}
			
		// forward the request to JSP
		dispatcher.forward(request, response);
	}




	private void removeFromShoppingCart(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
	{
		// Remove the selected songs and albums from the Cart
		
		// see which check boxes are checked
		String[] songCheckboxList= request.getParameterValues("removeSong");
		String[] albumCheckboxList= request.getParameterValues("removeAlbum");
		
		if( songCheckboxList != null )
		{
			for( String songId : songCheckboxList )
			{
				songCart.remove(songId);
			}
		}
		
		if( albumCheckboxList != null )
		{
			for( String albumId : albumCheckboxList )
			{
				albumCart.remove(albumId);
			}
		}

		// Once all duplicates are removed, calc the total price and construct the
		// list of songs and album to add to the cart page
		boolean noSongsOrAlbumsOnList = GetCartListAndTotalPrice(request);
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/shopping_cart.jsp");
		
		if(noSongsOrAlbumsOnList )
		{
			 // if both list at empty, show the "Chart is empty" page
			request.setAttribute("MESSAGE", 
					"Chart is empty!");
			dispatcher = request.getRequestDispatcher("/msg_and_back_to_search.jsp");
		}
			
		// forward the request to JSP
		dispatcher.forward(request, response);
	}



	private void shoppingCart(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		// see which check boxes are checked
		String[] songCheckboxList= request.getParameterValues("addSong");
		String[] albumCheckboxList= request.getParameterValues("addAlbum");
		List<String> dublicateSongs = new LinkedList<String>();
		
		
		// Search for duplicate songs and songs that already appear on 
		// an album.
		// IMPORTANT! It is essential that you check the songs first, because if
		// a song is chosen that is already on an album, then the 
		// "RemoveSongsAlreadyOnAlbum()"
		// function below will tidy it up and remove it, so it needs to be called
		// after all the checked songs are added!
		
		if( songCheckboxList != null )
		{
			for( String songId : songCheckboxList )
			{
				Song selectedSong = musicData.GetSong(songId);
				if( songCart.containsKey(songId) )
				{
					dublicateSongs.add(selectedSong.getTitle() );
				}
				else
				{
					// Also check to see if this song is already on a selected album
					if( (selectedSong != null) &&  !albumCart.containsKey( selectedSong.getAlbum().getId() ) )
					{
						songCart.put(songId,  selectedSong);
					}
					else
					{
						dublicateSongs.add(selectedSong.getTitle() );
					}
				}
			}
		}

		
		List<String> dublicateAlbums = new LinkedList<String>();
		if( albumCheckboxList != null)
		{
			for( String albumId : albumCheckboxList )
			{
				Album selectedAlbum = musicData.GetAlbum(albumId);
				if( albumCart.containsKey(albumId) )
				{
					dublicateAlbums.add(selectedAlbum.getTitle() );
				}
				else
				{
					//remove any song that this album contains from  the songCart
					dublicateSongs.addAll( RemoveSongsAlreadyOnAlbum( selectedAlbum ) );
					albumCart.put(albumId, selectedAlbum );
				}
			}
		}
		
		//GetCartListAndTotalPrice(request);
		boolean noSongsOrAlbumsOnList = GetCartListAndTotalPrice(request);
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/shopping_cart.jsp");
		
		if(noSongsOrAlbumsOnList )
		{
			 // if both list at empty, show the "Chart is empty" page
			request.setAttribute("MESSAGE", 
					"Chart is empty!");
			dispatcher = request.getRequestDispatcher("/msg_and_back_to_search.jsp");
		}
		else
		{
			// Add to a list of the duplicate album and songs
			if( !dublicateAlbums.isEmpty() )
			{
				String duplicateItemMsg = "The following albums were removed because they were already on in the cart: " 
						+ dublicateAlbums.toString();
				request.setAttribute("DUPLICATE_ALBUM", duplicateItemMsg);
			}
			
			if( !dublicateSongs.isEmpty() )
			{
				String duplicateItemMsg = "The following songs were removed because they were either already on in the cart "  
						+ "or on an album in the cart: " + dublicateSongs.toString();
				request.setAttribute("DUPLICATE_SONG", duplicateItemMsg);
			}
		}
			
		// forward the request to JSP
		dispatcher.forward(request, response);
	}


	private boolean GetCartListAndTotalPrice(HttpServletRequest request) 
	{
		// Once all duplicates are removed, calc the total price and construct the
		// list of songs and album to add to the cart page
		float totalPrice = 0;
		List<Album> albumList = new LinkedList<Album>();
		for( Album album: albumCart.values() )
		{
			totalPrice += Float.valueOf( album.getPrice() );
			albumList.add(album);
		}
		
		List<Song> songList = new LinkedList<Song>();
		for( Song song: songCart.values() )
		{
			totalPrice += Float.valueOf( song.getPrice() );
			songList.add(song);
		}

		// Set the total price attribute
		request.setAttribute("TOTAL_PRICE", totalPrice);
		
		// Set the list of songs and albums attributes
		request.setAttribute("SONG_LIST", songList);
		request.setAttribute("ALBUM_LIST", albumList);
		
		return (songList.isEmpty() && albumList.isEmpty() );
	}




	private List<String> RemoveSongsAlreadyOnAlbum( Album selectedAlbum ) 
	{
		List<String> dublicateSongs = new LinkedList<String>();
		List<Song> albumSongList = selectedAlbum.getSongList();
		
		for( Song song : albumSongList )
		{
			String songId = song.getSongId();
			if( songCart.containsKey( songId ) )
			{
				dublicateSongs.add( song.getTitle() );
				songCart.remove(songId);
			}
		}
		
		return dublicateSongs;
	}


	private void showResullts(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		// get the search text from form data
		String searchString = request.getParameter("searchText");
		String searchOption = request.getParameter("searchOption");
		boolean noMatchesFound = false;

		// Do the chosen search option:
		switch (searchOption) 
		{
			case "Song":
			{
				List<Song> songList = musicData.SearchSongs(searchString, SearchMode.TITLE);
				if( songList == null || songList.isEmpty() )
					noMatchesFound = true;
				else
					request.setAttribute("SONG_LIST", songList);
				break;
			}
			case "Album":
			{
				List<Album> albumList = musicData.SearchAlbums(searchString, SearchMode.TITLE);
				if( albumList == null || albumList.isEmpty() )
					noMatchesFound = true;
				else
					request.setAttribute("ALBUM_LIST", albumList);
				break;
			}
			case "Artist":
			{
				List<Song> songList = musicData.SearchSongs(searchString, SearchMode.ARTIST);
				List<Album> albumList = musicData.SearchAlbums(searchString, SearchMode.ARTIST);
				if( ( songList == null || songList.isEmpty() ) && ( albumList == null || albumList.isEmpty() ) )
				{
					noMatchesFound = true;
				}
				else
				{
					request.setAttribute("SONG_LIST", songList);
					request.setAttribute("ALBUM_LIST", albumList);
				}
				break;
			}
			case "Anything":
			{
				List<Song> songList = musicData.SearchSongs(searchString, SearchMode.ALL);
				List<Album> albumList = musicData.SearchAlbums(searchString, SearchMode.ALL);
				if( ( songList == null || songList.isEmpty() ) && ( albumList == null || albumList.isEmpty() ) )
				{
					noMatchesFound = true;
				}
				else
				{
					request.setAttribute("SONG_LIST", songList);
					request.setAttribute("ALBUM_LIST", albumList);
				}
				break;
			}
			default:
			{
				noMatchesFound = true;
			}
		}
		
		// get required request dispatcher
		RequestDispatcher dispatcher;
		if( noMatchesFound )
		{
			request.setAttribute("MESSAGE", 
					"Sorry, no matching datasets found! Try different search string.");
			dispatcher = request.getRequestDispatcher("/msg_and_cart_link.jsp");
		}
		else
		{
			dispatcher = request.getRequestDispatcher("/results.jsp");
		}
			
		// forward the request to JSP
		dispatcher.forward(request, response);
	}




	private void doSearch(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		// get request dispatcher
		RequestDispatcher dispatcher = 
					request.getRequestDispatcher("/search.jsp");

		// Get the list of 10 random songs
		List<Song> songList = musicData.GetRandomSongs(10);
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
