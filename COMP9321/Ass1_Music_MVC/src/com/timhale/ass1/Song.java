/**
 * 
 */
package com.timhale.ass1;

/**
 * @author Tim Hale
 *
 */

public class Song {
	
	private final String title;
	private final String artist;
	private final String price;
	private final String songId;
	private final Album album;
	private final String albumTitle;
	private final String genre;
	private final String publisher;
	private final String year;
	
	
	public Song(final String title, final String artist, 
			final String price, final String songId, final Album album ) {
		super();
		this.title = title;
		this.artist = artist;
		this.price = price;
		this.songId = songId;
		this.album = album;
		this.albumTitle = album.getTitle();
		this.genre = album.getGenre();
		this.publisher = album.getPublisher();
		this.year = album.getYear();
	}


	public String getTitle() {
		return title;
	}


	public String getArtist() {
		return artist;
	}


	public String getPrice() {
		return price;
	}


	public String getSongId() {
		return songId;
	}


	public Album getAlbum() {
		return album;
	}

	
	public String getAlbumTitle() {
		return albumTitle;
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

	
	
}


