<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>

<html>
<body>
	<div>
	<p>
		<h4> ${MESSAGE} </h4>
	</p>
	
	</div>
	
	<form action="ControllerServlet" method="GET">
		<input type="hidden" name="control_cmd" value="CART" />
			<p>
				<input type="submit" value="Shopping Cart" class="save" />
			</p>	
	</form>	
	
	<p>
		<a href="ControllerServlet">Back to search</a>
	</p>

</body>
</html>
