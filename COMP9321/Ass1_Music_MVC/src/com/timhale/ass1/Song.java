/**
 * 
 */
package com.timhale.ass1;

/**
 * @author Tim Hale
 *
 */

public class Song {
	
	private String title;
	private String artist;
	private String album;
	private String genre;
	private String publisher;
	private String year;
	private String price;
	private String id;
	
	
	public Song(String title, String artist, String album, String genre, String publisher, String year,
			String price, String id) {
		super();
		this.title = title;
		this.artist = artist;
		this.album = album;
		this.genre = genre;
		this.publisher = publisher;
		this.year = year;
		this.price = price;
		this.id = id;
	}


	public String getTitle() {
		return title;
	}


	public String getArtist() {
		return artist;
	}


	public String getAlbum() {
		return album;
	}


	public String getGenre() {
		return genre;
	}


	public String getPublisher() {
		return publisher;
	}


	public String getYear() {
		return year;
	}


	public String getPrice() {
		return price;
	}

	
	public String getId() {
		return id;
	}


	public void setTitle(String title) {
		this.title = title;
	}


	public void setArtist(String artist) {
		this.artist = artist;
	}


	public void setAlbum(String album) {
		this.album = album;
	}


	public void setGenre(String genre) {
		this.genre = genre;
	}


	public void setPublisher(String publisher) {
		this.publisher = publisher;
	}


	public void setYear(String year) {
		this.year = year;
	}


	public void setPrice(String price) {
		this.price = price;
	}
	
	
	public void setId(String id) {
		this.id = id;
	}
	
	


}
