<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>


<html>
<body>
	<div>
		<h2>Search results</h2>
		
		<form action="ControllerServlet" method="GET">
		
			<input type="hidden" name="control_cmd" value="CART" />
			
			<h3>Song results</h3>
		
			<table border="1">
			
				<tr>
					<th>Title </th>
					<th>Artist </th>
					<th>Album </th>
					<th>Genre </th>
					<th>Publisher </th>
					<th>Year </th>
					<th>Price </th>
					<th>Buy</th>
				</tr>
				
				<c:forEach var="tempSong" items="${SONG_LIST}">
																		
					<tr>
					<td> ${tempSong.title} </td>
					<td> ${tempSong.artist} </td>
					<td> ${tempSong.albumTitle} </td>
					<td> ${tempSong.genre} </td>
					<td> ${tempSong.publisher} </td>
					<td> ${tempSong.year} </td>
					<td> ${tempSong.price} </td>
					<td> <input type="checkbox" name="addSong" value=${tempSong.songId} > </td>
					</tr>
				
				</c:forEach>
				
			</table>

			<h3>Album results</h3>
		
			<table border="1">
			
				<tr>
					<th>Title </th>
					<th>Artist </th>
					<th>Genre </th>
					<th>Publisher </th>
					<th>Year </th>
					<th>Price </th>
					<th>Buy</th>
				</tr>
				
				<c:forEach var="tempAlbum" items="${ALBUM_LIST}">
																		
					<tr>
					<td> ${tempAlbum.title} </td>
					<td> ${tempAlbum.artist} </td>
					<td> ${tempAlbum.genre} </td>
					<td> ${tempAlbum.publisher} </td>
					<td> ${tempAlbum.year} </td>
					<td> ${tempAlbum.price} </td>
					<td> <input type="checkbox" name="addAlbum" value=${tempAlbum.id} > </td>
					</tr>
				</c:forEach>
			</table>
			
			<p>
				<input type="submit" value="Add to Cart" class="save" />
			</p>	
			
		</form>
	</div>
		
	<p>
		<a href="ControllerServlet">Back to search</a>
	</p>

</body>
</html>
