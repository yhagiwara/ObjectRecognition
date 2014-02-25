import java.util.HashMap;
import java.util.Map;

import org.web3d.parser.x3d.X3DFieldReader;
import org.web3d.util.SimpleStack;
import org.web3d.vrml.lang.*;
import org.web3d.vrml.nodes.*;
import org.web3d.vrml.nodes.proto.AbstractProto;
import org.web3d.vrml.nodes.proto.ProtoInstancePlaceHolder;
import org.web3d.vrml.nodes.proto.PrototypeDecl;
import org.web3d.vrml.parser.VRMLFieldReader;
import org.web3d.vrml.renderer.*;
import org.web3d.vrml.renderer.norender.NRProtoCreator;
import org.web3d.vrml.sav.*;

public class X3DParseEventHandler implements
	StringContentHandler,
	BinaryContentHandler,
	ProtoHandler,
	ScriptHandler,
	RouteHandler
{
	String profileName;
	int majorVersion;
	int minorVersion;

	VRMLNodeFactory nodeFactory = null;
	VRMLFieldReader fieldParser = new X3DFieldReader();

	CRVRMLScene scene = null;
	VRMLWorldRootNodeType root;

	// ノードスタック、フィールドインデックススタック
	VRMLNodeType[] nodeStack;
	int[] childIndexStack;
	int topOfStack;

	// 現在のノード
	private VRMLNodeType currentNode;

	// 現在のフィールドのインデックス
	private int currentFieldIndex;

	// ログ出力用
	private int indent;
	private boolean bDebugMode = false;

	// defマップ
	protected Map<String, VRMLNodeType> defMap;

	// protoマップ
	protected Map<String, VRMLNodeTemplate> protoMap;
		// protoMapは
		// <Proto宣言されたノード名>と<そのノードの定義(PrototypeDecl)>
		// のマップ．
		//
		// 例えば、list3-16.x3dの場合、
		// protoMap["ConeDisplay"]
		//	---> ConeDisplayのPrototypeDeclオブジェクト
		// となる．

	protected Map<String, VRMLNodeTemplate> externProtoMap;

	boolean inProtoDecl = false;
	boolean inProtoBody = false;

	boolean inExternProtoDecl = false;
	boolean inExternProtoURI = false;

	// 現在定義中のプロトタイプ宣言
	// PrototypeDeclオブジェクトがセットされる．
	AbstractProto currentProto;
	
	// protoMapのスタック
	// 存在意義がよくわからないが、念のために残しておく
	SimpleStack protoMapStack;

	private NodeTemplateToInstanceCreator protoCreator;

	public X3DParseEventHandler(CRVRMLScene scene)
	{
		// -------------------------------------------------
		//	VRML/X3Dツリーを保持する
		// -------------------------------------------------
		this.scene = scene;

		// -------------------------------------------------
		//	ノードスタック
		//	フィールドインデックススタック
		// -------------------------------------------------
		nodeStack = new VRMLNodeType[100];
		childIndexStack = new int[100];

		// -------------------------------------------------
		//	DEF/USE用
		// -------------------------------------------------
		defMap = new HashMap<String, VRMLNodeType>();

		// -------------------------------------------------
		//	PROTO/EXTERNPROTO用
		// -------------------------------------------------
		protoMap = new HashMap<String, VRMLNodeTemplate>();
		externProtoMap = new HashMap<String, VRMLNodeTemplate>();

		protoMapStack = new SimpleStack();
	}

	public void debugMode(boolean b)
	{
		bDebugMode = b;
	}

	private void print(String str)
	{
		if (bDebugMode)
		{
			String indentStr = X3DParser.getIndentSpace(indent);

			System.out.print(indentStr);
			System.out.println(str);
		}
	}

	public Map<String, VRMLNodeType> getDefMap()
	{
		return defMap;
	}

	// ----------------------------------------------------------
	//	Methods defined by ContentHandler
	// ----------------------------------------------------------
	public void setDocumentLocator(Locator loc) {
	}

	public void startDocument(
			String uri,
			String url,
			String encoding,
			String type,
			String version,
			String comment) throws SAVException, VRMLException
	{
		// -------------------------------------------------
		//	ノードファクトリー
		//
		//	"Shape", "Appearance"などのノード名から
		//	対応するノードオブジェクトを作って返してくれる
		// -------------------------------------------------
		nodeFactory = DefaultNodeFactory.newInstance("null");
		// 引数は"null", "java3d", "ogl"のどれか

		// -------------------------------------------------
		//	プロファイル名("VRML97" or "Immersive")
		//	バージョン番号(2.0 or 3.0)
		//	の調査と設定
		// -------------------------------------------------
		int real_enc = SceneMetaData.VRML_ENCODING;

		if(type.charAt(1) == 'V')
		{
			// VRML97(=2.0) or 1.0
			profileName = "VRML97";
			majorVersion = 2;
			minorVersion = 0;
		}
		else
		{
			// X3D(=VRML3.0)
			profileName = "Immersive";
			majorVersion = 3;

			real_enc = encoding.equals("utf8") ?
					   SceneMetaData.VRML_ENCODING :
					   SceneMetaData.XML_ENCODING;

			// マイナーバージョン番号を探す
			int dot_index = version.indexOf('.');
			String minor_num = version.substring(dot_index + 1);
			minorVersion = Integer.parseInt(minor_num);
		}

		// setSpecVersion()を先に呼ばないと失敗する
		nodeFactory.setSpecVersion(majorVersion, minorVersion);
		nodeFactory.setProfile(profileName);

		if ((majorVersion == 3) && (real_enc == SceneMetaData.XML_ENCODING))
		{
			fieldParser.setCaseSensitive(false);
		} 
		else
		{
			fieldParser.setCaseSensitive(true);
		}

		protoCreator = new NRProtoCreator(
				nodeFactory,
				null,
				majorVersion,
				minorVersion);

		// ワールドルート作成＆シーンにセット
		root = new MyWorldRoot();
		scene.setRootNode(root);

		// ノードスタック、フィールドインデックススタックの初期化
		topOfStack = 0;
		nodeStack[topOfStack] = root;
		childIndexStack[topOfStack] = root.getFieldIndex("children");

		// 現在のノード、インデックスフィールドの設定
		currentNode = nodeStack[topOfStack];
		currentFieldIndex = childIndexStack[topOfStack];
	}

	public void endDocument() throws SAVException, VRMLException
	{
		root.setupFinished();
	}

	public void startNode(String name, String defName) throws SAVException, VRMLException
	{
		if (defName != null) {
			print("<Node> "+name+" (defName="+defName+")");
		} else {
			print("<Node> "+name);
		}
		indent++;

		VRMLNodeType node = null;
		VRMLNodeTemplate proto_def = null;

		if (protoMap.containsKey(name))
		{
			// ここを通るのは
			// nameがユーザー定義ノード(Protoノード）名だった場合
//			print("<Node> "+name+" is ProtoDeclared node!!");

			proto_def = (VRMLNodeTemplate)protoMap.get(name);
				// Protoノード名から、ノード定義(PrototypeDecl)を取り出す
		}

		if (proto_def != null)
		{
			node = new ProtoInstancePlaceHolder(
					proto_def,
					majorVersion,
					minorVersion,
					false,
					protoCreator);
		}
		else
		{
			node = (VRMLNodeType)nodeFactory.createVRMLNode(name, false);
		}

		// ノードのクラス名を見てみる
//		Class c = node.getClass();
//		System.out.println("<<< "+c.getName()+" >>>");

		VRMLNodeType parentNode = nodeStack[topOfStack];
//		if ((inProtoDecl) && (parentNode == null))
		if (inProtoDecl)
		{
			// ここを通るのはProtoInterface内のフィールド宣言中に
			// ノード型のフィールドが出てきた場合.
			// このノード(=node)を
			// 親ノード(=PrototypeDecl)の currentFieldIndexフィールドの
			// 値としてセットする．
			((VRMLProtoDeclare)currentProto).addFieldNodeValue(currentFieldIndex, node);
		}
		else
		{
			parentNode.setValue(currentFieldIndex, node);
		}

		// 新しいノード(=node)を
		// (1) ノードスタックにpush
		// (2) カレントノードにする
		topOfStack++;
		nodeStack[topOfStack] = node;
		currentNode = node;

		// def名が指定されていれば、マップに登録
		if (defName != null)
		{
			defMap.put(defName, currentNode);
		}
	}

	public void endNode() throws SAVException, VRMLException
	{
		indent--;
		print("</Node>");

		// ノードスタックをpop
		nodeStack[topOfStack] = null;
		topOfStack--;

		// カレントノード、フィールドインデックスを更新
		if (topOfStack >= 0)
		{
			currentNode = nodeStack[topOfStack];
			currentFieldIndex = childIndexStack[topOfStack];
		}
		else
		{
			currentNode = null;
			currentFieldIndex = -1;
		}
	}

	public void startField(String name) throws SAVException, VRMLException
	{
		print("<Field> "+name);

		int index = currentNode.getFieldIndex(name);

		currentFieldIndex = index;
		childIndexStack[topOfStack] = index;
	}

	public void endField() throws SAVException, VRMLException
	{
		// print("</Field>");
	}

	public void fieldValue(String value) throws SAVException, VRMLException
	{
		print(value);

		if ((currentNode != null) && (currentFieldIndex >= 0))
		{
			VRMLFieldDeclaration decl = currentNode.getFieldDeclaration(currentFieldIndex);

			if (currentNode instanceof ProtoInstancePlaceHolder)
			{
				parseField(
						((VRMLProtoInstance)currentNode),
						currentFieldIndex,
						decl.getFieldType(),
						value);
			}
			else
			{
				parseField(
						currentNode,
						currentFieldIndex,
						decl.getFieldType(),
						value);
			}
		}
	}

	public void fieldValue(String[] values) throws SAVException, VRMLException
	{
		if ((currentNode != null) && (currentFieldIndex >= 0))
		{
			VRMLFieldDeclaration decl = currentNode.getFieldDeclaration(currentFieldIndex);

			parseField(
					currentNode,
					currentFieldIndex,
					decl.getFieldType(),
					values);
		}
	}

	public void profileDecl(String profileName)	throws SAVException, VRMLException
	{
	}

	public void componentDecl(String componentInfo)	throws SAVException, VRMLException
	{
	}

	public void metaDecl(String key, String value) throws SAVException, VRMLException
	{
	}

	public void importDecl(String inline, String exported, String imported) throws SAVException, VRMLException
	{
	}

	public void exportDecl(String defName, String exported) throws SAVException, VRMLException
	{
	}

	public void useDecl(String defName) throws SAVException, VRMLException
	{
		print("<Use> "+defName);

		VRMLNodeType node = (VRMLNodeType)defMap.get(defName);
		if (node != null)
		{
			currentNode.setValue(currentFieldIndex, node);
		}
	}

	// ----------------------------------------------------------
	//	Methods defined by ProtoHandler
	// ----------------------------------------------------------
	public void startProtoDecl(String name) throws SAVException, VRMLException
	{
		print("<ProtoDecl> "+name);
		indent++;

		if (inProtoDecl || inExternProtoDecl) {
			// ProtoDeclをネストすることはできない
			throw new VRMLException("Nested proto decl error");
		}

		PrototypeDecl proto = new PrototypeDecl(
				name,
				majorVersion,
				minorVersion,
				protoCreator);

		protoMap.put(name, proto);
		currentProto = proto;

		inProtoDecl = true;
	}

	public void endProtoDecl() throws SAVException, VRMLException
	{
		indent--;
		print("</ProtoDecl>");

		inProtoDecl = false;
	}

	public void protoFieldDecl(
			int access,
			String type,
			String name,
			Object value) throws SAVException, VRMLException
	{
		if (value != null) {
			print("<ProtoField> "+name+" "+type+" "+value.toString());
		} else {
			print("<ProtoField> "+name+" "+type);
		}

		if (!inProtoDecl && !inExternProtoDecl) {
			throw new VRMLException("proto field decl is not in proto decl!");
		}

		VRMLFieldDeclaration fieldDecl = new VRMLFieldDeclaration(access, type, name);

		int index = currentProto.appendField(fieldDecl);
			// currentProtoはPrototypeDeclオブジェクト
			// currentProto.appendField()
			//	--> AbstractProto.appendField()
			//
			// currentProtoに新しいフィールド定義を追加する．
			// 返り値は、今追加したフィールドに対するフィールドインデックス．
			// フィールドの値をセットするとき（下の★部分）で使う．

		currentFieldIndex = index;

		if ((value != null) && (currentProto instanceof VRMLProtoDeclare))
		{
			try
			{
				if (value instanceof String)
				{
					// ★
					parseField((VRMLProtoDeclare)currentProto, index, fieldDecl.getFieldType(), (String)value);
						// VRMLProtoDeclareはPrototypeDeclクラスの上位インターフェースの一つ
				}
				else
				{
					// ★
					parseField((VRMLProtoDeclare)currentProto, index, fieldDecl.getFieldType(), (String[])value);
				}
			}
			catch (FieldException fe)
			{
//				fe.setFieldName(fieldDecl.getName());
				throw fe;
			}
		}
	}

	// --------------------------------------------------------
	//	protoIsDecl
	//
	// このメソッドはstartFieldの直後に呼ばれ、
	// 直前に呼ばれたstartFieldのnameが、このprotoIsDeclのfieldNameに
	// connectする、ということを表す．
	//
	// (ex)
	// <Cone>
	//	 <IS>
	//	   <connect nodeField="height" protoField="coneHeight">
	//	 </IS>
	// </Cone>
	//
	// これは、プロトタイプで宣言されているフィールドconeHeightを
	// Coneノードのheightフィールドに接続せよ、という意味だが、
	// これは
	//
	// startField(name = "height")
	// protoIsDecl(fieldName = "coneHeight")
	//
	// の順でコールされることで表す．
	// --------------------------------------------------------
	public void protoIsDecl(String fieldName) throws SAVException, VRMLException	{
		print("<IS> "+fieldName);

		if (!(currentProto instanceof PrototypeDecl)) {
			throw new VRMLException("proto IS decl is not in proto!");
		}

		((PrototypeDecl)currentProto).addIS(fieldName, currentNode, currentFieldIndex);
	}

	public void startProtoBody() throws SAVException, VRMLException
	{
		print("<ProtoBody>");
		indent++;

		currentNode = (VRMLNodeType)((PrototypeDecl)currentProto).getBodyGroup();
			// getBodyGroup()は、現在定義中のプロトタイプ宣言(=currentProto)の
			// ProtoBody部を返す

		topOfStack++;
		nodeStack[topOfStack] = currentNode;

		currentFieldIndex = currentNode.getFieldIndex("children");
		childIndexStack[topOfStack] = currentFieldIndex;

		protoMapStack.push(protoMap);
		protoMap = new HashMap<String, VRMLNodeTemplate>();

		inProtoBody = true;
	}

	public void endProtoBody() throws SAVException, VRMLException
	{
		indent--;
		print("</ProtoBody>");

		currentNode.setupFinished();

		nodeStack[topOfStack] = null;
		topOfStack--;

		if (topOfStack >= 0)
		{
			currentNode = nodeStack[topOfStack];
			currentFieldIndex = childIndexStack[topOfStack];

			protoMap = (HashMap<String, VRMLNodeTemplate>)protoMapStack.pop();
		}
		else
		{
			currentNode = null;
			currentFieldIndex = -1;

			inProtoBody = false;
			protoMap = new HashMap<String, VRMLNodeTemplate>();
		}
	}

	public void startExternProtoDecl(String name) throws SAVException, VRMLException
	{
		print("<ExternProto> "+name);
		indent++;
	}

	public void endExternProtoDecl() throws SAVException, VRMLException
	{
		indent--;
		print("</ExternProto>");
	}

	public void externProtoURI(String[] values) throws SAVException, VRMLException
	{
		print("<ExternProtoURI>");

		for (int i=0; i<values.length; i++)
		{
			print(values[i]);
		}
	}

	// ----------------------------------------------------------
	//	Methods defined by BinaryContentHandler
	//
	//	バイナリデータをパースした際に呼ばれる
	//
	//	通常は.x3d, .wrl共にテキスト形式のものを扱うので、
	//	ここが呼ばれることはないと思うが念のために実装しておく
	//
	//	（バイナリのほうが文字列をパースして値を分解する手間が
	//	かからないので楽である）
	// ----------------------------------------------------------
	public void fieldValue(int value) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value);
	}

	public void fieldValue(int[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	public void fieldValue(boolean value) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value);
	}

	public void fieldValue(boolean[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	public void fieldValue(float value) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value);
	}

	public void fieldValue(float[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	public void fieldValue(long value) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value);
	}

	public void fieldValue(long[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	public void fieldValue(double value) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value);
	}

	public void fieldValue(double[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	public void fieldValue(String[] value, int len) throws SAVException, VRMLException
	{
		currentNode.setValue(currentFieldIndex, value, len);
	}

	// ----------------------------------------------------------
	//	Methods defined by ScriptHandler
	// ----------------------------------------------------------
	public void startScriptDecl() throws SAVException, VRMLException
	{
	}

	public void endScriptDecl() throws SAVException, VRMLException
	{
	}

	public void scriptFieldDecl(int access,
			String type,
			String name,
			Object value)
	throws SAVException, VRMLException
	{
	}

	// ----------------------------------------------------------
	//	Methods defined by RouteHandler
	// ----------------------------------------------------------
	public void routeDecl(String srcNodeName,
			String srcFieldName,
			String destNodeName,
			String destFieldName)
	throws SAVException, VRMLException
	{
	}

	// ------------------------------------------------------------------------
	// node ........ [o] 文字列をパースした結果が書き込まれるノード
	// index ....... [i] フィールドインデックス
	// fieldType ... [i] タイプ（SFInt32, MFDoubleなどの型)
	// value ....... [i] 入力文字列．これをパースした結果がnodeにセットされる
	// ------------------------------------------------------------------------
	private void parseField(VRMLNodeType node,
			int index,
			int fieldType,
			String value) {

		if(fieldType != FieldConstants.SFSTRING) {
			value = value.trim();

			if(value.length() == 0)
				return;
		}

//		Class c = node.getClass();
//		System.out.println("*** parseField(). node = " +c.getName()+" ***");

		switch(fieldType) {
		case FieldConstants.SFINT32:
			node.setValue(index, fieldParser.SFInt32(value));
			break;

		case FieldConstants.MFINT32:
			int[] i_val = fieldParser.MFInt32(value);
			node.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFFLOAT:
			node.setValue(index, fieldParser.SFFloat(value));
			break;

		case FieldConstants.SFTIME:
			node.setValue(index, fieldParser.SFTime(value));
			break;

		case FieldConstants.SFDOUBLE:
			node.setValue(index, fieldParser.SFDouble(value));
			break;

		case FieldConstants.MFTIME:
			double[] d_val = fieldParser.MFTime(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFDOUBLE:
			d_val = fieldParser.MFDouble(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFLONG:
			node.setValue(index, fieldParser.SFLong(value));
			break;

		case FieldConstants.MFLONG:
			long[] l_val = fieldParser.MFLong(value);
			node.setValue(index, l_val, l_val.length);
			break;

		case FieldConstants.SFBOOL:
			node.setValue(index, fieldParser.SFBool(value.toUpperCase()));
			break;

		case FieldConstants.SFROTATION:
			float[] f_val = fieldParser.SFRotation(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFROTATION:
			f_val = fieldParser.MFRotation(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFBOOL:
			boolean[] b_val = fieldParser.MFBool(value);
			node.setValue(index, b_val, b_val.length);
			break;

		case FieldConstants.MFFLOAT:
			f_val = fieldParser.MFFloat(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC2F:
			f_val = fieldParser.SFVec2f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3F:
			f_val = fieldParser.SFVec3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC4F:
			f_val = fieldParser.SFVec4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC2F:
			f_val = fieldParser.MFVec2f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC3F:
			f_val = fieldParser.MFVec3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC4F:
			f_val = fieldParser.MFVec4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3D:
			d_val = fieldParser.SFVec3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFVEC4D:
			d_val = fieldParser.SFVec4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC3D:
			d_val = fieldParser.MFVec3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC4D:
			d_val = fieldParser.MFVec4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFSTRING:
			node.setValue(index, fieldParser.SFString(value));
			break;

		case FieldConstants.MFSTRING:
			String[] s_val = fieldParser.MFString(value);
			node.setValue(index, s_val, s_val.length);
			break;

		case FieldConstants.SFIMAGE:
			i_val = fieldParser.SFImage(value);
			node.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.MFIMAGE:
			i_val = fieldParser.MFImage(value);
			node.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFCOLOR:
			f_val = fieldParser.SFColor(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLOR:
			f_val = fieldParser.MFColor(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFCOLORRGBA:
			f_val = fieldParser.SFColorRGBA(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLORRGBA:
			f_val = fieldParser.MFColorRGBA(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3F:
			f_val = fieldParser.SFMatrix3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX4F:
			f_val = fieldParser.SFMatrix4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX3F:
			f_val = fieldParser.MFMatrix3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX4F:
			f_val = fieldParser.MFMatrix4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3D:
			d_val = fieldParser.SFMatrix3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFMATRIX4D:
			d_val = fieldParser.SFMatrix4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX3D:
			d_val = fieldParser.MFMatrix3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX4D:
			d_val = fieldParser.MFMatrix4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFNODE:
		case FieldConstants.MFNODE:
			// in either case, this will be the value "NULL", so just
			// ignore it completely.
			break;

			default:
				/*
				errorReporter.warningReport("Unknown field type " +
											"provided to CRMainSceneBuilder! " +
											fieldType, null);
				 */
		}
	}

	private void parseField(VRMLNodeType node,
			int index,
			int fieldType,
			String[] value) {
		// If the length is zero, ignore the parse request
		if(value == null || value.length == 0)
			return;

		switch(fieldType) {
		case FieldConstants.SFINT32:
		case FieldConstants.SFFLOAT:
		case FieldConstants.SFTIME:
		case FieldConstants.SFDOUBLE:
		case FieldConstants.SFLONG:
		case FieldConstants.SFBOOL:
		case FieldConstants.SFSTRING:
			/*
				errorReporter.warningReport("Field not parsable as String[]. " +
											node.getVRMLNodeName() + " index: " +
											index + " val: " + value[0], null);
			 */
			break;

		case FieldConstants.MFINT32:
			int[] i_val = fieldParser.MFInt32(value);
			node.setValue(index, i_val, i_val.length);
			break;


		case FieldConstants.MFTIME:
			double[] d_val = fieldParser.MFTime(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFDOUBLE:
			d_val = fieldParser.MFDouble(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFLONG:
			long[] l_val = fieldParser.MFLong(value);
			node.setValue(index, l_val, l_val.length);
			break;

		case FieldConstants.SFROTATION:
			float[] f_val = fieldParser.SFRotation(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFROTATION:
			f_val = fieldParser.MFRotation(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFBOOL:
			boolean[] b_val = fieldParser.MFBool(value);
			node.setValue(index, b_val, b_val.length);
			break;

		case FieldConstants.MFFLOAT:
			f_val = fieldParser.MFFloat(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC2F:
			f_val = fieldParser.SFVec2f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3F:
			f_val = fieldParser.SFVec3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC4F:
			f_val = fieldParser.SFVec4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC2F:
			f_val = fieldParser.MFVec2f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC3F:
			f_val = fieldParser.MFVec3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC4F:
			f_val = fieldParser.MFVec4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3D:
			d_val = fieldParser.SFVec3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFVEC4D:
			d_val = fieldParser.SFVec4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC3D:
			d_val = fieldParser.MFVec3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC4D:
			d_val = fieldParser.MFVec4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFSTRING:
			String[] s_val = fieldParser.MFString(value);
			node.setValue(index, s_val, s_val.length);
			break;

		case FieldConstants.SFIMAGE:
			i_val = fieldParser.SFImage(value);
			node.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.MFIMAGE:
			i_val = fieldParser.MFImage(value);
			node.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFCOLOR:
			f_val = fieldParser.SFColor(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLOR:
			f_val = fieldParser.MFColor(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFCOLORRGBA:
			f_val = fieldParser.SFColorRGBA(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLORRGBA:
			f_val = fieldParser.MFColorRGBA(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3F:
			f_val = fieldParser.SFMatrix3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX4F:
			f_val = fieldParser.SFMatrix4f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX3F:
			f_val = fieldParser.MFMatrix3f(value);
			node.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX4F:
			f_val = fieldParser.MFMatrix4f(value);
			node.setValue(index, f_val, f_val.length);
			break;
		case FieldConstants.SFMATRIX3D:
			d_val = fieldParser.SFMatrix3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFMATRIX4D:
			d_val = fieldParser.SFMatrix4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX3D:
			d_val = fieldParser.MFMatrix3d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX4D:
			d_val = fieldParser.MFMatrix4d(value);
			node.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFNODE:
		case FieldConstants.MFNODE:
			// in either case, this will be the value "NULL", so just
			// ignore it completely.
			break;

			default:
				/*
				errorReporter.warningReport("Unknown field type " +
											"provided to CRMainSceneBuilder! " +
											fieldType, null);
				 */
		}
	}

	private void parseField(VRMLProtoDeclare proto,
			int index,
			int fieldType,
			String value) {

		if(fieldType != FieldConstants.SFSTRING) {
			value = value.trim();

			if(value.length() == 0)
				return;
		}

		switch(fieldType) {
		case FieldConstants.SFINT32:
			proto.setValue(index, fieldParser.SFInt32(value));
			break;

		case FieldConstants.MFINT32:
			int[] i_val = fieldParser.MFInt32(value);
			proto.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFFLOAT:
			proto.setValue(index, fieldParser.SFFloat(value));
			break;

		case FieldConstants.SFTIME:
			proto.setValue(index, fieldParser.SFTime(value));
			break;

		case FieldConstants.SFDOUBLE:
			proto.setValue(index, fieldParser.SFDouble(value));
			break;

		case FieldConstants.MFTIME:
			double[] d_val = fieldParser.MFTime(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFDOUBLE:
			d_val = fieldParser.MFDouble(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFLONG:
			proto.setValue(index, fieldParser.SFLong(value));
			break;

		case FieldConstants.MFLONG:
			long[] l_val = fieldParser.MFLong(value);
			proto.setValue(index, l_val, l_val.length);
			break;

		case FieldConstants.SFBOOL:
			proto.setValue(index, fieldParser.SFBool(value.toUpperCase()));
			break;

		case FieldConstants.SFROTATION:
			float[] f_val = fieldParser.SFRotation(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFROTATION:
			f_val = fieldParser.MFRotation(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFBOOL:
			boolean[] b_val = fieldParser.MFBool(value);
			proto.setValue(index, b_val, b_val.length);
			break;

		case FieldConstants.MFFLOAT:
			f_val = fieldParser.MFFloat(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC2F:
			f_val = fieldParser.SFVec2f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3F:
			f_val = fieldParser.SFVec3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC4F:
			f_val = fieldParser.SFVec4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC2F:
			f_val = fieldParser.MFVec2f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC3F:
			f_val = fieldParser.MFVec3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC4F:
			f_val = fieldParser.MFVec4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3D:
			d_val = fieldParser.SFVec3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFVEC4D:
			d_val = fieldParser.SFVec4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC3D:
			d_val = fieldParser.MFVec3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC4D:
			d_val = fieldParser.MFVec4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFSTRING:
			proto.setValue(index, fieldParser.SFString(value));
			break;

		case FieldConstants.MFSTRING:
			String[] s_val = fieldParser.MFString(value);
			proto.setValue(index, s_val, s_val.length);
			break;

		case FieldConstants.SFIMAGE:
			i_val = fieldParser.SFImage(value);
			proto.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.MFIMAGE:
			i_val = fieldParser.MFImage(value);
			proto.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFCOLOR:
			f_val = fieldParser.SFColor(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLOR:
			f_val = fieldParser.MFColor(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFCOLORRGBA:
			f_val = fieldParser.SFColorRGBA(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLORRGBA:
			f_val = fieldParser.MFColorRGBA(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3F:
			f_val = fieldParser.SFMatrix3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX4F:
			f_val = fieldParser.SFMatrix4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX3F:
			f_val = fieldParser.MFMatrix3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX4F:
			f_val = fieldParser.MFMatrix4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3D:
			d_val = fieldParser.SFMatrix3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFMATRIX4D:
			d_val = fieldParser.SFMatrix4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX3D:
			d_val = fieldParser.MFMatrix3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX4D:
			d_val = fieldParser.MFMatrix4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFNODE:
		case FieldConstants.MFNODE:
			// in either case, this will be the value "NULL", so just
			// ignore it completely.
			break;

		default:
			/*
errorReporter.warningReport("Unknown field type " +
							"provided to CRMainSceneBuilder! " +
							fieldType, null);
			 */
		}
	}

	private void parseField(VRMLProtoDeclare proto,
			int index,
			int fieldType,
			String[] value) {
		// If the length is zero, ignore the parse request
		if(value == null || value.length == 0)
			return;

//		System.out.println("+++ parseField for VRMLProtoDeclare is called !! +++");

		switch(fieldType) {
		case FieldConstants.SFINT32:
		case FieldConstants.SFFLOAT:
		case FieldConstants.SFTIME:
		case FieldConstants.SFDOUBLE:
		case FieldConstants.SFLONG:
		case FieldConstants.SFBOOL:
		case FieldConstants.SFSTRING:
			/*
				errorReporter.warningReport("Field not parsable as String[]. " +
											proto.getVRMLNodeName() + " index: " +
											index + " val: " + value[0], null);
			 */
			break;

		case FieldConstants.MFINT32:
			int[] i_val = fieldParser.MFInt32(value);
			proto.setValue(index, i_val, i_val.length);
			break;


		case FieldConstants.MFTIME:
			double[] d_val = fieldParser.MFTime(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFDOUBLE:
			d_val = fieldParser.MFDouble(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFLONG:
			long[] l_val = fieldParser.MFLong(value);
			proto.setValue(index, l_val, l_val.length);
			break;

		case FieldConstants.SFROTATION:
			float[] f_val = fieldParser.SFRotation(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFROTATION:
			f_val = fieldParser.MFRotation(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFBOOL:
			boolean[] b_val = fieldParser.MFBool(value);
			proto.setValue(index, b_val, b_val.length);
			break;

		case FieldConstants.MFFLOAT:
			f_val = fieldParser.MFFloat(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC2F:
			f_val = fieldParser.SFVec2f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3F:
			f_val = fieldParser.SFVec3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC4F:
			f_val = fieldParser.SFVec4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC2F:
			f_val = fieldParser.MFVec2f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC3F:
			f_val = fieldParser.MFVec3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFVEC4F:
			f_val = fieldParser.MFVec4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFVEC3D:
			d_val = fieldParser.SFVec3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFVEC4D:
			d_val = fieldParser.SFVec4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC3D:
			d_val = fieldParser.MFVec3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFVEC4D:
			d_val = fieldParser.MFVec4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFSTRING:
			String[] s_val = fieldParser.MFString(value);
			proto.setValue(index, s_val, s_val.length);
			break;

		case FieldConstants.SFIMAGE:
			i_val = fieldParser.SFImage(value);
			proto.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.MFIMAGE:
			i_val = fieldParser.MFImage(value);
			proto.setValue(index, i_val, i_val.length);
			break;

		case FieldConstants.SFCOLOR:
			f_val = fieldParser.SFColor(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLOR:
			f_val = fieldParser.MFColor(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFCOLORRGBA:
			f_val = fieldParser.SFColorRGBA(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFCOLORRGBA:
			f_val = fieldParser.MFColorRGBA(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX3F:
			f_val = fieldParser.SFMatrix3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.SFMATRIX4F:
			f_val = fieldParser.SFMatrix4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX3F:
			f_val = fieldParser.MFMatrix3f(value);
			proto.setValue(index, f_val, f_val.length);
			break;

		case FieldConstants.MFMATRIX4F:
			f_val = fieldParser.MFMatrix4f(value);
			proto.setValue(index, f_val, f_val.length);
			break;
		case FieldConstants.SFMATRIX3D:
			d_val = fieldParser.SFMatrix3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFMATRIX4D:
			d_val = fieldParser.SFMatrix4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX3D:
			d_val = fieldParser.MFMatrix3d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.MFMATRIX4D:
			d_val = fieldParser.MFMatrix4d(value);
			proto.setValue(index, d_val, d_val.length);
			break;

		case FieldConstants.SFNODE:
		case FieldConstants.MFNODE:
			// in either case, this will be the value "NULL", so just
			// ignore it completely.
			break;

			default:
				/*
				errorReporter.warningReport("Unknown field type " +
											"provided to CRMainSceneBuilder! " +
											fieldType, null);
				 */
		}
	}
}
