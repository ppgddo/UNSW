package com.timhale.ass1;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;




public class MusicXmlData {
	
	public enum SearchMode 
	{
	    TITLE, ARTIST, ALL
	}
	
	// private data;
	private HashMap<String, Album> albumDataMap;
	private HashMap<String, Song> songDataMap;
	private List<Song> songDataList;

	// public functions
	
	public MusicXmlData(InputSource xmlFile)
	throws ParserConfigurationException, SAXException, IOException
	{
		super();
		
		songDataList = new ArrayList<Song>();
		albumDataMap = new HashMap<String, Album>();
		songDataMap = new HashMap<String, Song>();
		
		
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder documentBuilder = factory.newDocumentBuilder();
		Document document = documentBuilder.parse(xmlFile);
		
		// Iterate through node list (album)
		NodeList albumNodeList = document.getElementsByTagName("albumList");
		for(int i = 0; i < albumNodeList.getLength(); i++)
		{
			try
			{
				Node albumNode = albumNodeList.item(i);
				List<Song> albumSongList = new ArrayList<Song>();
				NodeList albumChildNodes = albumNode.getChildNodes();
				Album album = null;
				String albumTitle = "";
				String artist = "";
				String genre = "";
				String publisher = "";
				String year = "";
				String price = "";
				String id = "";
				
				for(int j = 0; j < albumChildNodes.getLength(); j++)
				{
					Node item = albumChildNodes.item(j);
					
					//System.out.println( item.getNodeName() );
					
					if( item instanceof Element )
					{
						switch(item.getNodeName() )
						{
						case "artist":
							artist = item.getTextContent();
							break;
						case "title":
							albumTitle = item.getTextContent();
							break;
						case "genre":
							genre = item.getTextContent();
							break;
						case "publisher":
							publisher = item.getTextContent();
							break;
						case "year":
							year = item.getTextContent();
							break;
						case "price":
							price = item.getTextContent();
							break;
						case "ID":
							id = item.getTextContent();
							break;
						case "songList":
							NodeList songChildNodes = item.getChildNodes();
							if( album == null)
							{
								album = new Album(albumTitle, artist, genre, publisher, year, id);
							}
							
							Song nextSong = ParseSong(songChildNodes, album);
							songDataList.add(nextSong);
							songDataMap.put(nextSong.getSongId(), nextSong );
							albumSongList.add(nextSong);
							break;
						}
					}
				}
				
				album.setPrice(price);
				album.setSongList(albumSongList);
				albumDataMap.put(id, album);
			}
			catch (Exception e)
			{
				System.out.println("Error! Problem reading album, skipping to next one!");
			}
		}
		
	
		/*
		// TODO remove this debug code
		NodeList numberList = document.getElementsByTagName("artist");
		Node numberItem = numberList.item(0);

		System.out.println(numberItem.getTextContent() );
		System.out.println(numberItem.getTextContent() );
		*/
	
	}

	
	private Song ParseSong(NodeList songChildNodes, Album album) {
		
		String songTitle = "";
		String artist = "";
		String price = "";
		String songId = "";

		for (int j = 0; j < songChildNodes.getLength(); j++) {
			Node item = songChildNodes.item(j);

			// System.out.println( item.getNodeName() );

			if (item instanceof Element) {
				switch (item.getNodeName()) {
				case "artist":
					artist = item.getTextContent();
					break;
				case "title":
					songTitle = item.getTextContent();
					break;
				case "price":
					price = item.getTextContent();
					break;
				case "songID":
					songId = item.getTextContent();
					break;
				}
			}
		}
		
		return new Song(songTitle, artist, price, songId, album);
	}
	
	
	public List<Song> SearchSongs( String searchText, SearchMode searchMode )
	{
		// Find songs with the "searchText" in title
		
		List<Song> returnSongs = new LinkedList<Song>();
		HashSet<String> songsChosenMap = new HashSet<String>();
				
		// Iterate through each song and search the required fields
		for (Song nextSong : songDataList) 
		{
			// perform a case insensitive search - (?i) option enables this 
			boolean stringFound = false;
			if ( ( searchMode == SearchMode.TITLE ) || ( searchMode == SearchMode.ALL) )
			{
				// If searching for either the song title or all fields
				stringFound |= nextSong.getTitle().matches("(?i).*" + searchText + ".*");
			}
			if ( ( searchMode == SearchMode.ARTIST ) || ( searchMode == SearchMode.ALL) )
			{
				// If searching for either the song artist or all fields
				stringFound |= nextSong.getArtist().matches("(?i).*" + searchText + ".*");
			}
			if ( searchMode == SearchMode.ALL)
			{
				// If searching for all fields, then search the remaining fields not done above
				stringFound |= nextSong.getAlbumTitle().matches("(?i).*" + searchText + ".*");
				stringFound |= nextSong.getGenre().matches("(?i).*" + searchText + ".*");
				stringFound |= nextSong.getPrice().matches("(?i).*" + searchText + ".*");
				stringFound |= nextSong.getPublisher().matches("(?i).*" + searchText + ".*");
				stringFound |= nextSong.getYear().matches("(?i).*" + searchText + ".*");
			}

			if( stringFound && !songsChosenMap.contains(nextSong.getSongId() ) )
			{
				songsChosenMap.add(nextSong.getSongId() );
				returnSongs.add(nextSong);
			}
		}
		
		return returnSongs;
	}

	
	public List<Album> SearchAlbums( String searchText, SearchMode searchMode  )
	{
		// Find albums with the "searchText" in title
		
		List<Album> returnAlbums = new LinkedList<Album>();
		HashSet<String> albumsChosenMap = new HashSet<String>();
		
		// Iterate through each album and search the required fields
		for (Album nextAlbum : albumDataMap.values() ) 
		{
			// perform a case insensitive search - (?i) option enables this
			boolean stringFound = false;
			if ((searchMode == SearchMode.TITLE) || (searchMode == SearchMode.ALL)) 
			{
				// If searching for either the album title or all fields
				stringFound |= nextAlbum.getTitle().matches("(?i).*" + searchText + ".*");
			}
			if ((searchMode == SearchMode.ARTIST) || (searchMode == SearchMode.ALL)) 
			{
				// If searching for either the album artist or all fields
				stringFound |= nextAlbum.getArtist().matches("(?i).*" + searchText + ".*");
			}
			if (searchMode == SearchMode.ALL) 
			{
				// perform a case insensitive search - (?i) option enables this
				stringFound |= nextAlbum.getGenre().matches("(?i).*" + searchText + ".*");
				stringFound |= nextAlbum.getPrice().matches("(?i).*" + searchText + ".*");
				stringFound |= nextAlbum.getPublisher().matches("(?i).*" + searchText + ".*");
				stringFound |= nextAlbum.getYear().matches("(?i).*" + searchText + ".*");

			}

			if (stringFound && !albumsChosenMap.contains(nextAlbum.getId())) 
			{
				albumsChosenMap.add(nextAlbum.getId());
				returnAlbums.add(nextAlbum);
			}
		}
		
		return returnAlbums;
	}
	

	public List<Song> GetRandomSongs( int numberOfSongs )
	{
		// Get songs from XML database
		
		Random ran = new Random();
		
		HashSet<String> songsChosenMap = new HashSet<String>();
		List<Song> returnSongs = new LinkedList<Song>();
		
		int songCounter = 0;
		
		while( songCounter != numberOfSongs )
		{
			int x = ran.nextInt(songDataList.size() );
			Song nextSong = songDataList.get(x);
			if( !songsChosenMap.contains(nextSong.getSongId() ) )
			{
				songsChosenMap.add(nextSong.getSongId() );
				returnSongs.add(nextSong);
				songCounter++;
			}
		}
		
		return returnSongs;
	}


	public Album GetAlbum(String albumId) 
	{
		if( albumDataMap.containsKey(albumId) )
			return albumDataMap.get(albumId);
		else
			return null;
	}


	public Song GetSong(String songId) 
	{
		if( songDataMap.containsKey(songId) )
			return songDataMap.get(songId);
		else
			return null;
	}
}
