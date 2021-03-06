/**
 * @author Zhiqiang Ren 
 * date: Feb. 4th. 2012
 * 
 */

package aipackage;


public class Connection {
    
    public Connection(Node from, Node to, double weight) {
        m_from = from;
        m_to = to;
        m_weight = weight;
    }
    
    public Node getFromNode() {
        return m_from;
    }
    
    public Node getToNode() {
        return m_to;
    }
    
    public double getWeight() {
        return m_weight;
    }
    
    public void setWeight(double w) {
        m_weight = w;
    }
    
    public double getDeltaw() {
        return m_deltaw;
    }
    
    public void setDeltaw(double d) {
        m_deltaw = d;
    }
    
    private double m_weight;
    private double m_deltaw;

    private Node m_from;
    private Node m_to;

}
