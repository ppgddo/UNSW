package com.timhale.ass1;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;



public class MusicXmlData {
	private Document m_document;

	
	public MusicXmlData(InputSource xmlFile)
	throws ParserConfigurationException, SAXException, IOException{
		super();
		
		
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder documentBuilder = factory.newDocumentBuilder();
		m_document = documentBuilder.parse(xmlFile);
	
		/*
		// TODO remove this debug code
		NodeList numberList = m_document.getElementsByTagName("artist");
		Node numberItem = numberList.item(0);

		System.out.println(numberItem.getTextContent() );
		System.out.println(numberItem.getTextContent() );
		*/
	
	}

	
	public List<Song> GetRandomSongs( int numberOfSongs )
	{
		// TODO Get songs from XML database
		List<Song> returnSongs = new ArrayList<Song>();
		
		returnSongs.add( new Song("title", "artist", "album", "genre", "publisher", "year",
			"price", "1A" ) );
		returnSongs.add( new Song("title2", "artist2", "album2", "genre2", "publisher2", "year2",
				"price2", "2B" ) );
		
		return returnSongs;
	}
}
