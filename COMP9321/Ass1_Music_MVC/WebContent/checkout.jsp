<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>


<html>
<body>
	<div>
		<h2>Checkout</h2>

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
				</tr>
				
				<c:forEach var="tempAlbum" items="${ALBUM_LIST}">
																		
					<tr>
					<td> ${tempAlbum.title} </td>
					<td> ${tempAlbum.artist} </td>
					<td> ${tempAlbum.genre} </td>
					<td> ${tempAlbum.publisher} </td>
					<td> ${tempAlbum.year} </td>
					<td> ${tempAlbum.price} </td>
					</tr>
				</c:forEach>
			</table>

			<p>
				<table border="1">
					<tr>
					<td> Total Price: </td>
					<td> $ ${TOTAL_PRICE} </td>
					</tr>
				</table>
			</p>	
	</div>

		<p> </p>

		<table>
			<tbody>
				<td><label><b>Confirm Order: </b></label></td>
				
				<form action="ControllerServlet" method="GET">
					<input type="hidden" name="control_cmd" value="CONFRIM" />
						<td><input type="submit" value="Yes" class="save" /></td>
				</form>
		
				<form action="ControllerServlet" method="GET">
					<input type="hidden" name="control_cmd" value="CANCEL" />
						<td><input type="submit" value="No" class="save" /></td>
				</form>
			</tbody>
		</table>
	
	
		
	<p>
		<a href="ControllerServlet">Back to search</a>
	</p>

</body>
</html>
